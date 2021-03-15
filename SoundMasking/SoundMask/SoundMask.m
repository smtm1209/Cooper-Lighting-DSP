clc; clear; close all


%% environment noise
% PeopleTalking.mat file was sampled at 44100 Hz
load PeopleTalking.mat
Fs = 44100;
y = y(:,1)';

%% sample 10 seconds of noise
noise = y(1:Fs*10);
sample_size = length(noise); %number of samples to synthesize
FFT_size = 2^(nextpow2(sample_size));  % FFT size (power of 2)
noise = [noise,zeros(1,FFT_size-sample_size)];% Zero-padded noise for FFT input
X = fft(noise); % DFT
Xmag = abs(X);
Xdb = 20*log10(Xmag); % Spectral magnitude in dB


%% create a sound mask (-3dB)
dBfactor = -3;
Sdb = Xdb + dBfactor;
Smag = 10.^(Sdb./20);

wnoise = randn(1,length(Smag));
W = fft(wnoise);
W = W./abs(W);
S = Smag.*W;
s = ifft(S);

%% Play sound with sound mask
mask = s + noise;
sound(mask,Fs)

%% DFT plot of the environment noise and the sound mask.
w = linspace(0,2*pi,length(X));
plot(w,abs(X),'b')
hold on
plot(w,abs(S),'r')
legend({'noise', 'mask'})