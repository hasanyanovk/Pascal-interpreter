program example1;
var
    num1, num2, i: integer;
    Res, d: double;
begin
    num1 := 1;
    while (num1 < 10) do
    begin
        num1 := num1 * 2;
        d:= d + 0.4;
        Writeln(num1, d);
    end
end.
