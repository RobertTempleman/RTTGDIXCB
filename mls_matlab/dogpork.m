printf ("Hello,  dogpork!\n");

N=8;
nums=0:1:10;
powerindices = N-1:-1:0;
powers = 2.^powerindices;
disp("wad");
disp(powerindices );
disp(powers );

disp("wad");
for i=1:1:4
  disp(i);
  S(i, 1:4)=nums(i+1:i+4)
end

perm=['a','b','c','d','e','f','g','h','i','j','k','l']

butt(nums)=perm;

disp(butt);
disp("--------------");
disp(S);