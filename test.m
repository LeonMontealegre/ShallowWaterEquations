

N = 50;
N_T = 100;

dt = 1;
dx = 1/N;
K = 1;
g = -1;

h = zeros(N_T, N);
u = zeros(N_T, N);

h(1,:) = 10;
u(1,:) = 0;

h(2,:) = 10;
u(2,:) = 0;

figure(1);
for t=2:N_T
    for x=1:N-1
        h(t+1, x) = h(t-1,x) - 2*u(t,x)*dt/dx * (h(t,x+1) - h(t,x-1)) - 2*h(t,x)*dt/dx * (u(t,x+1) - u(t,x-1)) + K*dt/dx^2 * (h(t-1,x+1) + h(t-1,x-1) - 2*h(t-1,x));
        u(t+1, x) = u(t-1,x) - 2*u(t,x)*dt/dx * (u(t,x+1) - u(t,x-1)) - 2*g*dt/dx * (h(t,x+1) - h(t,x-1));
    end
    
    plot(1:N-1, h);
end