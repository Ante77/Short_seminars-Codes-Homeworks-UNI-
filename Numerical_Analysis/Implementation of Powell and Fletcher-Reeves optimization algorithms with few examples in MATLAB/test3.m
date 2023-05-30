## Funkcija Powell_algorithm2 testira Primjer 8.5.6. sa str. 381.
## Za minimiziranje funkcija f(alfa)=f(x_i+alfa*s_i) i f(alfa)=f(x_n+alfa*s_n) koristi
## funkciju Matlab/Octave funkciju fminbnd.

## Funkcija ispisuje sve vektore x, s, te sve vrijednosti alfa kroz zadatak, kako je 
## napisano i u danom primjeru. Sve vrijednosti se poklapaju s knjigom.

## Po�to se uvjet za "konvergenciju" da je razlika izme?u dvije uzastopne iteracije
## manja od epsilon, uvijek se izvr�i jedan korak petlje vi�e nego �to je napisano u knjizi.

clear
clc

f=@defquadfja;

epsilon=1e-5;

x01=[2.5;2];
x02=[2;2];
x03=[1;2];

format short e
epsilon
format short
[xmin1,it1]=Powell_algorithm2 (f,x01,epsilon)
[xmin2,it2]=Powell_algorithm2 (f,x02,epsilon)
[xmin3,it3]=Powell_algorithm2 (f,x03,epsilon)

minimum=f(xmin1)

n=50;
F=zeros(n+1,n+1);
x1=-5;
x2=5;
y1=-5;
y2=5;
dx=(x2-x1)/n;
dy=(y2-y1)/n;
x=x1:dx:x2;
y=y1:dy:y2;

for i=1:n+1
  for j=1:n+1
    F(i,j)=f([x(i),y(j)]);
  end
end

[X,Y]=meshgrid(x,y);
figure()
surf(X,Y,F);