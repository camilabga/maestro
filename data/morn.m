filename = "sample02_square_01.csv";
N = csvread(filename);

x = (N(:,2) - 320)/(320);
y = (480 - N(:,3))/(240);

out = [N(:,1) x y];

csvwrite(filename,out);