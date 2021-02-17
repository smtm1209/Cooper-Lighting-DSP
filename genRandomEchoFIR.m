function [h, b, a] = genRandomEchoFIR(N, loss)

% echo designed based on  
% https://www.mathworks.com/help/audio/examples/acoustic-echo-cancellation-aec.html
[b, a] = cheby2(4, 40, [0.1 0.7]+0.04.*randn(1, 2));
b = b .* 10^(-loss / 20);
% fvtool(P_B, P_A);
% fvtool(P_B, P_A, "Analysis", "Impulse");

imp = [1; zeros(N-1, 1)];
h = filter(b, a, imp);

end