program example3;
const
    Pi: double = 3.1415926;
var
    num1, num2: integer;
    Res, d: double;
begin
    num1 := 5;
    Write('Enter an even value: ');
    Read(num2);
    Write('Enter float value: ');
    Read(d);
    if (num2 mod 2 = 0) then
    begin
        Res := (num1 - (num2 * 5) div 2) / (d * 2);
        Writeln('Result = ', Res);
    end
    else
        Writeln('Wrong input');
end.