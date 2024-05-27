program example2;
const
    part :double = 2.1;
var
    i: integer;
    res: double;
begin
    res := 1;
    for i := 0 to 20 do
    begin
        res := res * part;
        Writeln('i = ', i);
    end;
end.