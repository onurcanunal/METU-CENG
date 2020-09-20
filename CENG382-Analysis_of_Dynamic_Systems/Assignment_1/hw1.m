t = 0:0.1:10;
Rt = cos(t);
Jt = sin(t);
figure; hold on
title ("R(t) and J(t) versus t");
xlabel ("t");
ylabel ("R(t), J(t)");
a1 = plot(t,Rt); 
M1 = 'R(t)';
a2 = plot(t,Jt); 
M2 = 'J(t)';
legend([a1;a2], M1, M2);

figure; hold on
plot(Jt, Rt);
title ("R(t) versus J(t)");
xlabel ("J(t)");
ylabel ("R(t)");

k1 = 0:1:10;
delta_t = 0.5;
Rk = (1 / 2) * ((1 + i*delta_t).^k1 + (1 - i * delta_t) .^ k1);
Jk = (1 / 2 * delta_t) * ((1 + i*delta_t).^k1 + (1 - i * delta_t) .^ k1 - (1 + i*delta_t).^(k1+1) - (1 - i * delta_t) .^ (k1+1));
figure; hold on
plot(Jk, Rk);
title ("R(k) versus J(k) with respect to delta t=0.5");
xlabel ("J(k)");
ylabel ("R(k)");

k2 = 0:1:10;
delta_t = 0.1;
Rk = (1 / 2) * ((1 + i*delta_t).^k2 + (1 - i * delta_t) .^ k2);
Jk = (1 / 2 * delta_t) * ((1 + i*delta_t).^k2 + (1 - i * delta_t) .^ k2 - (1 + i*delta_t).^(k2+1) - (1 - i * delta_t) .^ (k2+1));
figure; hold on
plot(Jk, Rk);
title ("R(k) versus J(k) with respect to delta t=0.1");
xlabel ("J(k)");
ylabel ("R(k)");

k3 = 0:1:10;
delta_t = 0.01;
Rk = (1 / 2) * ((1 + i*delta_t).^k3 + (1 - i * delta_t) .^ k3);
Jk = (1 / 2 * delta_t) * ((1 + i*delta_t).^k3 + (1 - i * delta_t) .^ k3 - (1 + i*delta_t).^(k3+1) - (1 - i * delta_t) .^ (k3+1));
figure; hold on
plot(Jk, Rk);
title ("R(k) versus J(k) with respect to delta t=0.01");
xlabel ("J(k)");
ylabel ("R(k)");