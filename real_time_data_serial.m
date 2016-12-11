%run('clean');
clear
close all
fclose(instrfind);
 
s = serial('COM5'); %assigns the object s to serial port
 
set(s, 'InputBufferSize', 512); %number of bytes in inout buffer
set(s, 'FlowControl', 'hardware');
set(s, 'BaudRate', 115200);
set(s, 'Parity', 'none');
set(s, 'DataBits', 8);
set(s, 'StopBit', 1);
set(s, 'Timeout',10);
%clc;
 
disp(get(s,'Name'));
prop(1)=(get(s,'BaudRate'));
prop(2)=(get(s,'DataBits'));
prop(3)=(get(s, 'StopBit'));
prop(4)=(get(s, 'InputBufferSize'));
 
disp(['Port Setup Done!!',num2str(prop)]);

fig=figure('Unit','normalized','OuterPosition',[1 0 0.5 0.9]);

fopen(s);           %opens the serial port
disp('Running');

freq_start=387;
freq_stop=464;
freq_step=0.05;
totalpoints=256*6+5;

syncBytes=4;
SR=zeros(syncBytes,1);%shift register for detecting sync bytes
data=-150*ones(totalpoints,1);
freq=(freq_start:freq_step:freq_stop).';
dBm_threthold=-80;  % threthold to display center freq
rssi_offset=74;

while(1)  %Runs for 200 cycles - if you cant see the symbol, it is "less than" sign. so while (t less than 200)
 
   a =fread(s,1); % one byte a time;
   SR(2:end)=SR(1:end-1);
   SR(1)=a;
   if sum(SR==128*ones(syncBytes,1))==syncBytes
       % start capturing
       for i =1:totalpoints
           a =fread(s,1); % one byte a time;
           SR(2:end)=SR(1:end-1);
           SR(1)=a;
           %convert to dBm and store in data(i)
           if a>=128
               a=(a-256)/2 - rssi_offset;
           else
               a=a/2-rssi_offset;
           end
           if a>data(i)
               data(i)=a;   % to accumulate the spectrum
           end
%            data(i)=a;
       end
       plot(freq,data);
       xlim([freq_start freq_stop]);
       ylim([-120 -20]);
       grid on;
       title(['Spectrum-',num2str(freq_start),'MHz', ' to ' num2str(freq_stop),'MHz']);
       ylabel('dBm');
       xlabel('Freq/MHz');
       
       for i=1:totalpoints
           if data(i)>dBm_threthold
               text(freq(i),data(i)+5,[num2str(freq(i)),'MHz, ',num2str(data(i)),'dBm']);
           end
       end
       drawnow;
   end
   

end
 
fclose(s); %close the serial port