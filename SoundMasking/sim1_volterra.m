%% Housekeeping

clear;
close all;

%% Real-World Model Parameters

% Load Original Audio
mp3_fname = "./SoundMasking/dogBarking.mp3";
[raw_sig, fs_original] = audioread(mp3_fname);

% Echo Parameters
echo_taps = 64;
echo_loss_db = -5; % voltage dB

% Basic Testing Filter Parameters
sys_taps = 128;
sys_gain_db = 10; % voltage dB
sys_delay = 10; % 10 tap delay FIR

% Processing "Delay" Filter
proc_delay = 10;

% Adaptive Filter Taps
p = 64;

% NLMS Parameters
nlms_mu = 0.8e-2;
nlms_eps = 1e-15;

% Volterra Parameters
volterra_mu0 = 1e-3;
volterra_mu1 = 1e-2;
volterra_mu2 = 1e-4;

% Sound Masking Parameters
dBfactor = -3;

% Room Impulse Parameters
[h_room, fs_room] = audioread('./IRs/W00x00y.wav');


%% System Parameters 

% System Sampling Frequency
fs = 8e3; % system @ 8kHz

%% Pre-Processing

% Get Sampling Period
Ts = 1/fs;

% Generate Echo and Plot
[h_echo, b_echo, a_echo] = genRandomEchoFIR(echo_taps, echo_loss_db);
% freqz(b_echo, a_echo, 2000);
% h_echo = resample(h_room, fs, fs_room);
% p = numel(h_echo);
figure;
plot(h_echo);
title("Echo FIR");
xlabel("Taps");
drawnow;


% Generate System Filter
h_sys = db2mag(sys_gain_db).*[zeros(sys_delay, 1); ...
    1; zeros(sys_taps - 1 - sys_delay, 1)];

% Generate Processing "Delay" Filter
h_delay = [zeros(proc_delay, 1); 1];

% Resample Signal
raw_sig = mean(raw_sig, 2);
% raw_sig = raw_sig(1:40000);
sig = resample(raw_sig, fs, fs_original);
sig = sig(1:150000);

figure;
tt = (1/fs)*(0:numel(sig)-1);
plot(tt, sig);
title("Incoming Signal");
ylabel("Amplitude");
xlabel("Time [s]");
drawnow


%% System Simulation Without Echo

out_noecho = zeros(numel(sig), 1);
in_noecho = zeros(numel(sig), 1);
y_win_noecho = zeros(p, 1);
e_win_noecho = zeros(p, 1);

for k=1:numel(sig)
    if mod(k, 10000) == 0
        fprintf("No Echo....[%d/%d]\n", k, numel(sig));
    end
    
    if k - proc_delay >= 1
        a_k = sig(k-proc_delay);
    else
        a_k = 0;
    end
    
    % x_k = sample received from microphone at start of
    % adaptive filter
    x_k = a_k;
    
    % e_k = output of adaptive filter
    e_k = x_k;
    e_win_noecho = [e_k; e_win_noecho(1:end-1, :)];
    in_noecho(k) = e_k;
    
    % apply system filter
    Efft = fft(e_win_noecho);
    Emag = abs(Efft);
    Edb = 20*log10(Emag);
    
    Sdb = Edb + dBfactor;
    Smag = 10.^(Sdb/20);
    
    w = randn(numel(Smag), 1);
    W = fft(w);
    W = W./abs(W);
    S = Smag .* W;
    s = ifft(S);
    y_k = s(end);
    
    % prep for next iteration    
    out_noecho(k) = y_k;
    y_win_noecho = [y_k; y_win_noecho(1:end-1, :)];

end

%% System Simulation Without Echo Cancellation

out_nocanc = zeros(numel(sig), 1);
in_nocanc = zeros(numel(sig), 1);
y_win_nocanc = zeros(p, 1);
e_win_nocanc = zeros(p, 1);

h_echo_delay = conv(h_echo, h_delay);
h_echo_delay = h_echo_delay(1:p);

for k=1:numel(sig)
    if mod(k, 10000) == 0
        fprintf("No Canc....[%d/%d]\n", k, numel(sig));
    end
    
    if k - proc_delay >= 1
        a_k = sig(k-proc_delay);
    else
        a_k = 0;
    end
    
    % x_k = sample received from microphone at start of
    % adaptive filter
    x_k = a_k + h_echo_delay.' * y_win_nocanc;
    
    % e_k = output of adaptive filter
    e_k = x_k;
    e_win_nocanc = [e_k; e_win_nocanc(1:end-1, :)];
    in_nocanc(k) = e_k;
    
    % apply system filter
    Efft = fft(e_win_nocanc);
    Emag = abs(Efft);
    Edb = 20*log10(Emag);
    
    Sdb = Edb + dBfactor;
    Smag = 10.^(Sdb/20);
    
    w = randn(numel(Smag), 1);
    W = fft(w);
    W = W./abs(W);
    S = Smag .* W;
    s = ifft(S);
    y_k = s(end);
    
    % prep for next iteration    
    out_nocanc(k) = y_k;
    y_win_nocanc = [y_k; y_win_nocanc(1:end-1, :)];

end


%% System Simulation With Echo Cancellation

out_canc = zeros(numel(sig), 1);
in_canc = zeros(numel(sig), 1);
pre_canc = zeros(numel(sig), 1);

y_win_canc = zeros(p, 1);
e_win_canc = zeros(p, 1);

lms_fir = zeros(p, 1);
lms_fir_hist = zeros(p, numel(sig));

h_echo_delay = conv(h_echo, h_delay);
h_echo_delay = h_echo_delay(1:p);

for k=1:numel(sig)
    if mod(k, 10000) == 0
        fprintf("LMS....[%d/%d]\n", k, numel(sig));
    end
    
    if k - proc_delay >= 1
        a_k = sig(k-proc_delay);
    else
        a_k = 0;
    end
    
    % x_k = sample received from microphone at start of
    % adaptive filter
    x_k = a_k + h_echo_delay.' * y_win_canc;
    pre_canc(k) = x_k;
    
    % e_k = output of adaptive filter
    lms_fir_hist(:, k) = lms_fir;
    e_k = x_k - y_win_canc.' * lms_fir;
    
    % Update LMS Filter based on e_k
    lms_fir = lms_fir + nlms_mu .* conj(e_k) ...
        .*  y_win_canc / (nlms_eps + y_win_canc'*y_win_canc);
%     lms_fir(1) = 0;
    
    e_win_canc = [e_k; e_win_canc(1:end-1, :)];
    in_canc(k) = e_k;
    
    % apply system filter
    Efft = fft(e_win_canc);
    Emag = abs(Efft);
    Edb = 20*log10(Emag);
    
    Sdb = Edb + dBfactor;
    Smag = 10.^(Sdb/20);
    
    w = randn(numel(Smag), 1);
    W = fft(w);
    W = W./abs(W);
    S = Smag .* W;
    s = ifft(S);
    y_k = s(end);
    
    % prep for next iteration    
    out_canc(k) = y_k;
    y_win_canc = [y_k; y_win_canc(1:end-1, :)];
    
end

%% System Simulation With Echo Cancellation

out_volterra = zeros(numel(sig), 1);
in_volterra = zeros(numel(sig), 1);
pre_volterra = zeros(numel(sig), 1);

y_win_volterra = zeros(p, 1);
e_win_volterra = zeros(p, 1);

volterra_const = 0;
volterra_linear = zeros(p, 1);
volterra_quadratic = zeros(p);

volterra_linear_hist = zeros(p, numel(sig));

h_echo_delay = conv(h_echo, h_delay);
h_echo_delay = h_echo_delay(1:p);

for k=1:numel(sig)
    if mod(k, 10000) == 0
        fprintf("Volterra....[%d/%d]\n", k, numel(sig));
    end
    
    if k - proc_delay >= 1
        a_k = sig(k-proc_delay);
    else
        a_k = 0;
    end
    
    % x_k = sample received from microphone at start of
    % adaptive filter
    x_k = a_k + h_echo_delay.' * y_win_volterra;
    pre_volterra(k) = x_k;
    
    % e_k = output of adaptive filter
    volterra_linear_hist(:, k) = volterra_linear;
    e_k = x_k ...
        - volterra_const ...
        - volterra_linear.' * y_win_volterra ...
        - y_win_volterra.' * volterra_quadratic * y_win_volterra;
    
    % Update Volterra Filter based on e_k
    volterra_const = volterra_const ...
        + volterra_mu0 .* e_k;
    volterra_linear = volterra_linear ...
        + volterra_mu1 .* e_k .* y_win_volterra;
    volterra_quadratic = volterra_quadratic ...
        + volterra_mu2 .* e_k .* (y_win_volterra * y_win_volterra.');
    
    e_win_volterra = [e_k; e_win_volterra(1:end-1, :)];
    in_volterra(k) = e_k;
    
    % apply system filter
    Efft = fft(e_win_volterra);
    Emag = abs(Efft);
    Edb = 20*log10(Emag);
    
    Sdb = Edb + dBfactor;
    Smag = 10.^(Sdb/20);
    
    w = randn(numel(Smag), 1);
    W = fft(w);
    W = W./abs(W);
    S = Smag .* W;
    s = ifft(S);
    y_k = s(end);
    
    % prep for next iteration    
    out_volterra(k) = y_k;
    y_win_volterra = [y_k; y_win_volterra(1:end-1, :)];
    
end

%% Plot Various Signals

ymin = min([out_noecho; out_nocanc; out_canc; out_volterra]);
ymax = max([out_noecho; out_nocanc; out_canc; out_volterra]);
figure;
subplot(4,1,1);
plot(tt, out_noecho);
ylim([ymin ymax]);
title("Output Signal Without Any Echo");
xlabel("Time [s]");
ylabel("Amplitude");
subplot(4,1,2);
plot(tt, out_nocanc);
ylim([ymin ymax]);
title("Output Signal Without NLMS");
xlabel("Time [s]");
ylabel("Amplitude");
subplot(4,1,3);
plot(tt, out_canc);
ylim([ymin ymax]);
title("Output Signal With NLMS");
xlabel("Time [s]");
ylabel("Amplitude");
subplot(4,1,4);
plot(tt, out_volterra);
ylim([ymin ymax]);
title("Output Signal With Volterra LMS");
xlabel("Time [s]");
ylabel("Amplitude");


%% Plot Convergence of NLMS

caxis_min = min([min(h_echo) min(lms_fir_hist(:))]);
caxis_max = max([max(h_echo) max(lms_fir_hist(:))]);

figure;
subplot(3, 1, [1 2]);
imagesc(0:(p-1), 20*(1:size(lms_fir_hist, 2)), lms_fir_hist.');
caxis([caxis_min caxis_max]);
title("Learned Echo FIR Weights");
xlabel("FIR Taps");
ylabel("Time Steps");
colorbar
subplot(3, 1, 3);
% imagesc(0:(p-1), 0:(numel(aa)-1), abs(P_h_rls_tbl.'));
imagesc(0:(p-1), [0 1], h_echo_delay.');
caxis([caxis_min caxis_max]);
title("Actual Echo FIR Weights");
xlabel("FIR Taps");
colorbar

%% Plot Convergence of Volterra Linear Part

caxis_min = min([min(h_echo) min(volterra_linear_hist(:))]);
caxis_max = max([max(h_echo) max(volterra_linear_hist(:))]);

figure;
subplot(3, 1, [1 2]);
imagesc(0:(p-1), 20*(1:size(volterra_linear_hist, 2)), volterra_linear_hist.');
caxis([caxis_min caxis_max]);
title("Learned Volterra Linear Portion");
xlabel("FIR Taps");
ylabel("Time Steps");
colorbar
subplot(3, 1, 3);
% imagesc(0:(p-1), 0:(numel(aa)-1), abs(P_h_rls_tbl.'));
imagesc(0:(p-1), [0 1], h_echo_delay.');
caxis([caxis_min caxis_max]);
title("Actual Echo FIR Weights");
xlabel("FIR Taps");
colorbar

%% Plot Convergance L2

figure;
plot(tt, sqrt(vecnorm(lms_fir_hist - h_echo_delay, 2)));
title("RMSE of NLMS Calculated Echo vs Actual Echo");
ylabel("RMSE");
xlabel("Time [s]");

%% Play Audio for each method
fprintf("Raw Signal");
original_signal_player = audioplayer(sig, fs);
playblocking(original_signal_player);

fprintf("Dog With Noise Cancellation and No Echo\n");
noecho_player = audioplayer(out_noecho+sig, fs);
playblocking(noecho_player);

fprintf("Dog With Noise Cancellation and No Echo Cancellation\n");
nocanc_player = audioplayer(out_nocanc+sig, fs);
playblocking(nocanc_player);

fprintf("Dog With Noise Cancellation and LMS Echo Cancellation\n");
canc_player = audioplayer(out_canc+sig, fs);
playblocking(canc_player);

fprintf("Dog With Noise Cancellation and Volterra Echo Cancellation\n");
volterra_player = audioplayer(out_volterra+sig, fs);
playblocking(volterra_player);









