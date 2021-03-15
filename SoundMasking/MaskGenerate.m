function newMask = MaskGenerate (d, pastMask)
%inputs:
%   pastMask: Mask generated previously
%   d: input from mic

%output: 
%   newMask: new mask generated

%   Assumes d and pastMask is of same length and powers of 2.
    %disp(d)
    %% check if pastMask is all zeros (which will be the case during initial setup)
    if ~all(pastMask) 
        e = d;
        
    %if paskMask was defined, do adaptive filtering.
    else 
        %apatation factor
        mu = 0.002; 
        
        %initial filter weight guess
        N = 10;
        w = zeros(1,N); 
        
        %adaptive filter
        [dhat, e, w] = nlms(pastMask,d,mu,w);
    end
    
    %% FFT of e (estimated environment noise)
    E = fft(e);
    Emag = abs(E); %Spectral magnitude
    Edb = 20*log10(Emag); %Spectral magnitude in dB
    
    %% Create a sound mask
    dBfactor = +3;
    Mdb = Edb + dBfactor;
    Mmag = 10.^(Mdb./20);
    wnoise = randn(1,length(Mmag));
    W = fft(wnoise);
    W = W./abs(W);
    M = Mmag.*W;
    m = ifft(M);
    newMask = m;
    
    

