set term postscript enhanced
set out 'hist.ps'

set xlabel 't'
set ylabel 'Kinetic energy'
set grid
p 'history.dat' u 1:2 t 'Steady KE' w l lw 2, \
  'history.dat' u 1:3 t 'Unsteady KE' w l lw 2

set title 'Evolution of perturbation energy'
set xlabel 't'
set ylabel 'Perturbation energy'
set grid
p 'history.dat' u 1:4 w l lw 2 t 'Velocity', \
  'history.dat' u 1:5 w l lw 2 t 'Temperature'

set title 'Evolution of perturbation energy'
set xlabel 't'
set ylabel 'Perturbation energy'
set logscale y
set grid
p 'history.dat' u 1:4 w l lw 2 t 'Velocity', \
  'history.dat' u 1:5 w l lw 2 t 'Temperature'

set title 'Evolution of control'
set xlabel 't'
set ylabel 'Velocity, temperature
set y2label 'Heat flux'
set y2tics
unset logscale y
unset grid
p 'control.dat' u 1:2 t 'Velocity'    w l lw 2, \
  'control.dat' u 1:3 t 'Temperature' w l lw 2, \
  'control.dat' u 1:4 t 'Heat flux'   w l lw 2 axes x1y2
