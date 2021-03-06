      program main
      implicit none
      include 'param.h'
      real, allocatable :: q(:),xc(:),xv(:),dx(:),dt(:)
      real, allocatable :: res(:)
      real, allocatable :: qb(:),qb1(:)
      real, allocatable :: resb(:), resbold(:)
      real, allocatable :: mmat(:), amat(:), bmat(:), cmat(:)
      integer nc
      integer i, niter,iter, stage, mode
      real :: maxspeed, CFL , alpha
      real :: xi_1, tol_conv
      real :: cost, costb1,costb,residue,residue0
      real :: q1,res1,resb1,dSdxi_1
      real :: dxl, dxr
      integer :: code

      call read_input(mode, xi_1, CFL, niter, tol_conv)

      call bc()


c     Set up mesh

      open(10, file='grid.dat', status='old')
      read(10,*) nc
      allocate( q(nc),xc(nc),xv(nc+1),dx(nc),dt(nc) )
      allocate( res(nc) )
      allocate( qb(nc) , qb1(nc))
      allocate( resb(nc), resbold(nc) )
      do i=1,nc+1
         read(10,*) xv(i)
      enddo
      close(10)

      do i=1,nc
         xc(i) = 0.5*(xv(i) + xv(i+1))
         dx(i) = xv(i+1) - xv(i)
      enddo

      if(mode.eq.2)then
         print*,'Reading primal solution ...'
         open(10, file='primal.dat')
         read(10,*) (q(i),i=1,nc)
         close(10)
         print*,'Reading adjoint solution ...'
         open(10, file='adjoint.dat')
         read(10,*) (resb(i),i=1,nc)
         close(10)
         costb = 1.0
         qb    = 0.0
         call costfun_bq(nc, q, qb, cost, costb, dx)
         call residu_bq(nc, q, qb, res, resb, xc, xv, dx, 1)
         print*,'Saving adjoint residual ...'
         open(10, file='a_residual.dat')
         write(10,'(e24.14)') (qb(i), i=1,nc)
         close(10)
         stop
      endif

c     initialize conditions

      open(10, file='flo.dat', status='old')
      do i=1,nc
         read(10,*) xc(i), q(i)
      enddo
      close(10)


      costb = 1.0
      qb    = 0.0
      call costfun_bq(nc, q, qb, cost, costb, dx)
      resb = 0.0
      qb1(1:nc)  =  qb(1:nc)

      open(10, file='initadj.dat')
      do i=1,nc
         write(10,1000) xc(i), resb(i)
      enddo
      close(10)

c     matrices for TDMA
      include 'impmat.f'

      do iter=1,niter
         resbold(1:nc)=resb(1:nc)
         qb(1:nc)=qb1(1:nc)
         call residu_bq(nc, q, qb, res, resb, xc, xv, dx, 0)
         qb(1:nc) = -qb(1:nc)
         do i=1,nc
            qb(i) = qb(i) + mmat(i)*resbold(i)
         enddo
         call tridag(amat,bmat,cmat,qb,resb,nc,code)
         if(code.ne.0) stop "tridiag failed"
         residue = 0.0;
         do i=1,nc
            residue = residue + ((resb(i)-resbold(i))/dt(i))**2
         enddo
         residue = sqrt(residue)
         if(iter.eq.1) residue0 = residue
         residue = residue/residue0
         print*,iter,residue
         if(abs(residue)<tol_conv) exit
      enddo


      q1=0.0
      res1=0.0
      costb1=0.0
      do i=1,nc
         dSdxi_1=0.0
         resb1=1.0
         CALL SOURCEIS_BA(q1,res1,resb1,xc(i),dx(i),xi_1,dSdxi_1)
         costb1=costb1+dSdxi_1*resb(i)
      enddo

      print*,'Gradients= ',costb1

      open(10, file='adj.dat')
      open(11, file='exact_adj.dat')
      open(12, file='adjoint.dat')
      do i=1,nc
         write(10,'(2e21.13)')xc(i), resb(i)
         write(11,*)xc(i)
         write(12,'(e24.14)') resb(i)
      enddo
      close(10)
      close(11)
      close(12)

 1000  format(3(1X,E12.6))

      deallocate( q )
      deallocate( res )
      deallocate( qb )
      deallocate( resb )

      stop
      end
