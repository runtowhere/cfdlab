function fem_50 ( )
   parameters;
%
%  Read the nodal coordinate data file.
%
  load coordinates.dat;
%
%  Read the triangular element data file.
%
  eval ( 'load elements3.dat;', 'elements3=[];' );
%
%  Read the Neumann boundary condition data file.
%
  eval ( 'load neumann.dat;', 'neumann=[];' );
%
%  Read the Dirichlet boundary condition data file.
%
  eval ( 'load dirichlet.dat;', 'dirichlet=[];' );

  nNodes = size(coordinates,1)
  nTri = size(elements3,1)


  A = sparse ( nNodes, nNodes );
  M = sparse ( nNodes, nNodes );
%
%  Assembly.
%
  for j = 1 : nTri
%   Stiffness matrix
    A(elements3(j,:),elements3(j,:)) = A(elements3(j,:),elements3(j,:)) ...
      + nu*stima3(coordinates(elements3(j,:),:));
%   Mass matrix
    M(elements3(j,:),elements3(j,:)) = M(elements3(j,:), elements3(j,:)) ...
       + det([1,1,1;coordinates(elements3(j,:),:)'])*[2,1,1;1,2,1;1,1,2]/24;
  end
%
%  Determine which nodes are associated with Dirichlet conditions.
%
  BoundNodes = unique ( dirichlet );

% Find nodes for which x = a
  ControlNodes = find ( coordinates(:,1) - a > -eps );

  FreeNodes = setdiff ( 1:nNodes, BoundNodes );

  B = -A(FreeNodes, ControlNodes);
  A = -A(FreeNodes, FreeNodes);
  M = M(FreeNodes, FreeNodes);

  nBoundNodes = size(BoundNodes,1)
  nFreeNodes = size(FreeNodes,2)
  nControlNodes = size(ControlNodes,1)

% Add shift
  A = A + omega * M;

% One dimensional control u(y,t) = v(t) sin(pi*y)
  B = B * sin(pi*coordinates(ControlNodes,2));

% Compute eigenvalues and eigenfunctions
% [V,D] = eigs(A, M, 5, 'LR');
% e = diag(D);
% figure(10)
% plot(real(e),imag(e),'o')
% fprintf(1,'Most unstable eigenvalue (numerical) = %f\n', e(1))
% fprintf(1,'Most unstable eigenvalue (exact)     = %f\n', -pi^2/40 + omega)
% ef = zeros(nNodes,1);
% ef(FreeNodes) = V(:,1);
% show ( elements3, coordinates, full ( ef ) );
% pause(2)

% Observations on left boundary
  nObs = 3;
  H = sparse ( nObs, nNodes );
  ymid = 0.5*( coordinates(neumann(:,1),2) + coordinates(neumann(:,2),2) );
  ymin = [0.20, 0.50, 0.80];
  ymax = [0.25, 0.55, 0.85];
  for j=1:nObs
    ie = find(ymid > ymin(j) & ymid < ymax(j));
    ds = abs(coordinates(neumann(ie,2),2) - coordinates(neumann(ie,1),2));
    H(j,neumann(ie,1)) = H(j,neumann(ie,1)) + 0.5*ds';
    H(j,neumann(ie,2)) = H(j,neumann(ie,2)) + 0.5*ds';
    ds = sum(ds);
    H(j,:) = H(j,:)/ds;
  end
  H = H(:, FreeNodes);

  Z = sparse ( nNodes, 1 );        % Complete solution
  z = sparse ( nFreeNodes, 1 );    % Interior solution
  u = sparse ( nControlNodes, 1 ); % Control vector

  dt = 0.01;
  Nt = 100;
  A1 = M - dt*A;
  Z(:)= -cos(pi*coordinates(:,1)) .* sin(pi*coordinates(:,2));
  z(:) = Z(FreeNodes);
  u(:) = Z(ControlNodes);
  v    = 1;
  show ( elements3, coordinates, full ( Z ) );
  pause(2)

% Time loop
  t = 0;
  for it = 1:Nt
     z = A1 \ (M*z + dt*B*v);
     t = t + dt;
     Z(FreeNodes) = z;
     Z(ControlNodes) = v*u;
     if mod(it,10) == 0
      show ( elements3, coordinates, full ( Z ) );
      pause(1)
     end
     y = H*z;
     fprintf(1,'Time = %e %e %e %e\n', t, y(1), y(2), y(3))
  end
%
%  Graphic representation.
%
  show ( elements3, coordinates, full ( Z ) );

end
