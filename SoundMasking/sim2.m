clear; clc;
close all

h = [1 -1 0.5 -0.5 0.2 0.1 0.1]; %the room response

%% Samples
[sample1,Fs] = audioread('PeopleTalking.mp3',[1, 524288]);
[sample2, Fs] = audioread('dogBarking.mp3',[1, 524288]);
[sample3 ,Fs] = audioread('PeopleTalking2.mp3',[1+524288*5, 524288+524288*5]);

sample1 = (sample1(:,1))';
sample2 = 0.5*(sample2(:,1))';
sample3 = 5*(sample3(:,1))';
mask = zeros(1,524288);

%% generate mask
mask1 = MaskGenerate(sample1 + filter(h,1,mask), mask);
mask = mask1;
mask2 = MaskGenerate(sample1 + sample2 + filter(h,1,mask), mask);
mask = mask2;
mask3 = MaskGenerate(sample3 + filter(h,1,mask), mask);
mask = mask3;


%% plots
omega = linspace(0,2*pi,length(mask1));
figure(1)
subplot(2,1,1)
plot(omega,abs(fft(sample1))); title('DFT of the actual noise')
subplot(2,1,2)
plot(omega,abs(fft(mask1))); title('DFT of the mask')


figure(2)
subplot(2,1,1)
plot(omega, abs(fft(sample1+sample2))); title('DFT of the actual noise')
subplot(2,1,2)
plot(omega,abs(fft(mask2))); title('DFT of the mask')

figure(3)
subplot(2,1,1)
plot(omega, abs(fft(sample3))); title('DFT of the actual noise')
subplot(2,1,2)
plot(omega,abs(fft(mask3))); title('DFT of the mask')

%% sound
%sound(sample1+mask1,Fs) %first iteration
%sound(sample1+sample2+mask2,Fs) %second iteration
%sound(sample3+mask3,Fs) %third iteration
