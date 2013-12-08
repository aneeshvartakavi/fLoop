clear all; close all; clc;

loopDir = '/Users/Cam/Desktop/loop_classes_all/';

fs = 44100;

tempos = [115 115 115 115 115 115 ...
            115 115 115 115 115 115 ...
            128 128 128 128 128 128 ...
            128 128 128 128 128 128 ...
            140 140 120 120 82 81 ];

names = dir(strcat(loopDir, '*.wav'));
names = struct2cell(names);
names = names(1,:)';

numNames = size(names,1);
features = zeros(16,numNames);

allTempos = [];

for i=1:numNames
    fileName = char(names(i)) 
    
    x = wavread(strcat(loopDir, fileName));
    
    tempo = round(60 * 8 * fs / size(x, 1));
    
    if tempo > 160;
        tempo = tempo/2;
    elseif tempo < 50;
        tempo = tempo*2;
        
    end
    
    tempo
    
    allTempos = [allTempos tempo];
    
end




