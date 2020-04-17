-- Caclulates the fibonacci number given by the user
begin
  integer temp;

  -- uses temp to calculate fib of temp
  -- result is in temp
  proc fib
  begin
    integer x, y, z;
    if temp > 1 do
      x := temp;
      temp := x - 1;
      call fib;
      y := temp;
      temp := x - 2;
      call fib;
      z := temp;
      temp := y + z;
    else if ~(temp > 1) do
      skip;
    end if;
  end proc;

  -- read in number, call fib, write result
  read temp;
  call fib;
  write temp;
end.
