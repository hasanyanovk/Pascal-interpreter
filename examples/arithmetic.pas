program example6;
const
    Pi: double = 3.1415926;
    str: string = 'MyString';
var
    num1, num2: integer;
    Res, d: double;
    path: string;
begin
    Write('Enter your string:');
    Read(path);
    Write('Enter two integer values: ');
    Read(num1, num2);
    Write('Enter float value: ');
    Read(d);
    Writeln('num1 + num2 = ', num1+num2);
    Writeln('num1 - num2 = ', num1-num2);
    Writeln('num1 * d = ', num1*d);
    Writeln('num1 / d = ', num1/d);
    Writeln('num1 mod num2 = ', num1 mod num2);
    Writeln('num1 div num2 = ', num1 div num2);
end.