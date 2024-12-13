# Union-Find with area + perimeter
class Regions
    def initialize(w, h, i)
        @w = w
        @h = h
        @e = w.times.map{|y|h.times.map{|x|{
            :val => i[y][x],
            :id => [y,x],
            :area => 1,
            :perimeter => 0
        }}}
    end

    def inc_perimeter(x, y)
        ry, rx = @e[y][x][:id]
        @e[ry][rx][:perimeter]+=1
    end

    def union(xn, yn, xo, yo)
        id_old = @e[yo][xo][:id]
        id_new = @e[yn][xn][:id]
        return if id_old == id_new
        id_old, id_new = id_new, id_old if (id_old <=> id_new) < 0
        yn, xn = id_new
        yo, xo = id_old
        @e[yn][xn][:area] += @e[yo][xo][:area]
        @e[yn][xn][:perimeter] += @e[yo][xo][:perimeter]
        @h.times{|y|@w.times{|x|@e[y][x][:id]=id_new if @e[y][x][:id]==id_old}}
    end

    def all_regions
        @h.times.flat_map{|y|@w.times.select{|x|@e[y][x][:val]!=?. && @e[y][x][:id]==[y,x]}.map{|x|@e[y][x]}}
    end

    def all_regions_perimeter_discount
        all = all_regions.map{|r|r.merge({:perimeter_discount=>0})}
        lookup = all.each_with_index.map{|r,i|[r[:id],i]}.to_h
        # horizontal segments
        1.upto(@h-1){|y|
            prev_t = prev_c = nil
            1.upto(@w-1){|x|
                t=@e[y-1][x][:id]
                c=@e[y][x][:id]
                if t != c then
                    if prev_t != t
                        all[lookup[t]][:perimeter_discount]+=1 if lookup[t]
                        prev_t = t
                    end
                    if prev_c != c
                        all[lookup[c]][:perimeter_discount]+=1 if lookup[c]
                        prev_c = c
                    end
                else
                    prev_t = prev_c = nil
                end
            }
        }
        # vertical segments
        1.upto(@w-1){|x|
            prev_t = prev_c = nil
            1.upto(@h-1){|y|
                t=@e[y][x-1][:id]
                c=@e[y][x][:id]
                if t != c then
                    if prev_t != t
                        all[lookup[t]][:perimeter_discount]+=1 if lookup[t]
                        prev_t = t
                    end
                    if prev_c != c
                        all[lookup[c]][:perimeter_discount]+=1 if lookup[c]
                        prev_c = c
                    end
                else
                    prev_t = prev_c = nil
                end
            }
        }
        all
    end
end

# Input + padding
i = [*$<].map{|s|".#{s.strip}.".chars}
w = i[0].size
i = [ [?.]*w ] + i + [ [?.]*w ]
h = i.size

# Part 1 (44s)
r = Regions.new w, h, i
1.upto(h-1){|y|1.upto(w-1){|x|
    cur = i[y][x]
    left = i[y][x-1]
    top = i[y-1][x]
    if cur != left then
        r.inc_perimeter(x-1, y)
        r.inc_perimeter(x, y)
    else
        r.union(x-1, y, x, y)
    end
    if cur != top then
        r.inc_perimeter(x, y-1)
        r.inc_perimeter(x, y)
    else
        r.union(x, y-1, x, y)
    end
}}
p r.all_regions.sum{|e|e[:area]*e[:perimeter]}
# Part 2
p r.all_regions_perimeter_discount.sum{|e|e[:area]*e[:perimeter_discount]}
