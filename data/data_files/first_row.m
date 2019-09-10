filename = "sample02_square_01.csv";
N = csvread(filename);

M = N(:, 1);

for i=1:1045
    M(i) =i/1045;
end

out = [M N(:,2) N(:,3)];

csvwrite(filename,out);