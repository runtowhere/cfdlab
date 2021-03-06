subroutine savevort(t, omg)
   use comvar
   implicit none

   real    :: t, omg( 1:nx+1,  1:ny+1)

   integer :: i, j
   real    :: x, y 
   character(len=512) :: filename

   filename = 'omg'
   call getfilename(filename, fileid_omg)

   open(10,file=trim(filename))
   write(10,*)'TITLE = "vortex flow"'
   write(10,*)'VARIABLES = "x", "y", "Vorticity"'
   write(10,*)'ZONE STRANDID=1, SOLUTIONTIME=',t,', I=',nx+1,', J=',ny+1,&
              ', DATAPACKING=POINT'

   ! vorticity is computed at vertices (not at cell centers)
   do j=1,ny+1
      do i=1,nx+1
         x = xmin + (i-1)*dx
         y = ymin + (j-1)*dy

         write(10,'(3E24.14)') x, y, omg(i,j)

      enddo
   enddo

   close(10)

end subroutine savevort
