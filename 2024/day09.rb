# Input
input = gets.chars
# Part 1 - expand input
d=[]
id=0
input.each_with_index{|c,i|d += [i%2==0 ? id : -1] * c.to_i; id+=i%2}
# Part 1 - defrag (0.46s)
b=0
e=d.size-1
while b<e do
    if d[b]!=-1 then
        b+=1
    elsif d[e]==-1 then
        e-=1
    else
        d[b]=d[e]
        b+=1
        e-=1
    end
end
e-=1 while d[e]==-1
# Part 1 - checksum
p d[0..e].each_with_index.sum{|f,i|f*i}
# Part 2 - find files and holes
f=[]
h=[]
pos=0
id=0
input.each_with_index{|c,i|
    h << [pos, c.to_i] if i%2==1
    f << [pos, c.to_i, id] if i%2==0
    id+=i%2
    pos+=c.to_i
}
# Part 2 - defrag + checksum (1.90s)
checksum=0
f.reverse.each{|fpos, fsize, fid|
    t = h.find_index{|b|b[0]<fpos && b[1]>=fsize}
    if t then
        fpos=h[t][0]
        h[t][0]+=fsize
        h[t][1]-=fsize
    end
    checksum+=fid*(fpos...fpos+fsize).sum
}
p checksum
