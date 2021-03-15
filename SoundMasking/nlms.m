function [dhat, e, w] = nlms (x,d,mu,w)
%x: speakr signal
%d: target signal
%mu: beta 
%w: initial weight guess
%dhat: filter output(predicted value)

if (nargin < 4)
    % initial filter weight guess
    N = 10;
    w = zeros(1,N);

else
    N = length(w);

end

dhat = zeros(size(d));
e = zeros(size(d));

% now implement the adaptive filter

for n = N:length(x)
 xx = x(n:-1:n-(N-1));
 % produce filtered output sample

 dhat(n) = w * x(n:-1:n-(N-1))';
 % update the filter coefficients
 e(n) = d(n) - dhat(n);
 2*mu*xx*e(n)'/(xx*xx');
 w = w+2*mu*xx*e(n)'/(xx*xx');
end