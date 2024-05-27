program example4;
const
    part :double = 2.1;
var
    i: integer;
    res: double;
begin
    Write('Enter i: ');
    Read(i);
    if(i > 0) then
    begin
        if(i > 100) then
        begin
            res := 123.7;
        end
        else
            res := 732.1;
    end
    Writeln('I = ', i);
    Writeln('res = ', res);
end.