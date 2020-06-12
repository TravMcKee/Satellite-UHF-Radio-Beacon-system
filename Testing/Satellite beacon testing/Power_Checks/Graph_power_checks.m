clear; clc;
load Power_Checks

figure
plot(Times,Channel1V*1000)
hold on
plot(Times3,Channel1V3*1000)
plot(Times4,Channel1V4*1000)
plot(Times1,Channel1V1*100)
plot(Times2,Channel1V2*200)
ax=gca; ax.FontSize=16;
grid minor;
title('Arduino Pro Mini power mode current consumption measurements','fontSize',24);
xlabel('Time (sec)','fontSize',18);
ylabel('Current (mA)','fontSize',18);
legend('measured using 1\Omega resistor with 5V through the APM regulator','measured using 1\Omega resistor with 3.3V through the APM regulator','measured using 1\Omega resistor with 3.3V bypassing the APM regulator','measured using 10\Omega resistor','measured using 20\Omega resistor','fontSize',12);
xlim([-147,80]); ylim([-1 70]); 
