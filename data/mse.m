f1 = "sample02_square_00.csv";
f2 = "sample02_square_01.csv";

sem = csvread(f1);
com = csvread(f2);

MSE0 = immse(sem, com);