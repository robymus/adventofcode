# Part 1
s=STDIN.read
p s.scan(/mul\((\d{1,3}),(\d{1,3})\)/).sum{|x,y|x.to_i*y.to_i}
# Part 2
sum=0
enabled=true
s.split(/(do\(\)|don't\(\))/).each{|cmd|
    case cmd
    when "do()"
        enabled=true
    when "don't()"
        enabled=false
    else
        sum+=cmd.scan(/mul\((\d{1,3}),(\d{1,3})\)/).sum{|x,y|x.to_i*y.to_i} if enabled
    end
}
p sum