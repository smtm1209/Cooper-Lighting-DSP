%% Housekeeping

clear;
close all;

%% Real-World Model Parameters

% Load Original Audio
mp3_fname = "./signals/coffee_beep.mp3";
[raw_sig, fs_original] = audioread(mp3_fname);

% Echo Parameters
echo_taps = 128;
echo_loss_db = 0; % voltage dB

% Basic Testing Filter Parameters
sys_taps = 128;
sys_gain_db = 10; % voltage dB
sys_delay = 10; % 10 tap delay FIR

% Processing "Delay" Filter
proc_delay = 10;

% Adaptive Filter Taps
p = 128;

% NLMS Parameters
nlms_mu = 1e-5;
nlms_eps = 1e-15;

% Sound Masking Parameters
dBfactor = -3;

%% System Parameters 

% System Sampling Frequency
fs = 8e3; % system @ 8kHz

%% Pre-Processing

% Get Sampling Period
Ts = 1/fs;

% Generate Echo and Plot
[h_echo, b_echo, a_echo] = genRandomEchoFIR(echo_taps, echo_loss_db);
freqz(b_echo, a_echo, 2000);

% Generate System Filter
h_sys = db2mag(sys_gain_db).*[zeros(sys_delay, 1); ...
    1; zeros(sys_taps - 1 - sys_delay, 1)];

% Generate Processing "Delay" Filter
h_delay = [zeros(proc_delay, 1); 1];

% Resample Signal
raw_sig = mean(raw_sig, 2);
sig = resample(raw_sig, fs, fs_original);


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
    lms_fir = lms_fir - nlms_mu .* conj(e_k) ...
        .*  y_win_canc / (nlms_eps + y_win_canc'*y_win_canc);
    lms_fir(1) = 0;
    
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

%% Play Audio for each method
original_signal_player = audioplayer(sig, fs);
playblocking(original_signal_player);

noecho_player = audioplayer(out_noecho, fs);
playblocking(noecho_player);

nocanc_player = audioplayer(out_nocanc, fs);
playblocking(nocanc_player);

canc_player = audioplayer(out_canc, fs);
playblocking(canc_player);








