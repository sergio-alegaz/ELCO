rAux = 0:1000000;
Vcc = 5;
rFlex_0 = 25000;
rFlex_1 = 70000;

v0_0 = Vcc*(rAux./(rAux+rFlex_0));
v0_1 = Vcc*(rAux./(rAux+rFlex_1));

vDiff = abs(v0_0 - v0_1);

subplot(1,2,1);
plot(rAux,v0_0,'m');
hold on
plot(rAux,v0_1,'g');
hold off;
grid;

subplot(1,2,2);
plot(rAux, vDiff, 'r');
grid;

[peak,location] = findpeaks(vDiff,rAux);