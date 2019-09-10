filename = "maestro3D-1.csv";
N = csvread(filename);

N = sortrows(N, 1);

csvwrite(filename,N);