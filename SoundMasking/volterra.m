function [dhat, e, w0, w1, w2] = volterra(x,d,mu,w0,w1,w2)
%x: speaker signal
%d: target signal
%mu: vector of volterra learning rates 
%w0: initial weight guess - constant
%w1: initial weight guess - linear
%w2: initial weight guess - quadratic
%dhat: filter output(predicted value)

if (nargin < 4)
    % initial filter weight guess
    N = 10;
    w0 = 0;
    w1 = zeros(N,1);
    w2 = zeros(N);

else
    w1 = reshape(w1, [], 1);
    x = reshape(x, [], 1);
    N = length(w1);
end

dhat = zeros(size(d));
e = zeros(size(d));

% now implement the adaptive filter

for n = N:length(x)
 xx = x(n:-1:n-(N-1));
 % produce filtered output sample

 dhat(n) = w0 + w1'*xx + xx'*w2*xx;
 % update the filter coefficients
 e(n) = d(n) - dhat(n);
 
 w0 = w0 + mu(1).*e(n);
 w1 = w1 + mu(2).*e(n).*xx;
 w2 = w2 + mu(3).*e(n).*(xx*xx.');
 
end