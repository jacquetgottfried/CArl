function sol = ArlequinOutput( u, model, opt )
% ARLEQUINOUTPUT to construct solutions in format adapted to each of the
% models
%
%  u = ArlequinOutput( model )
%
% copyright: Laboratoire MSSMat, Ecole Centrale Paris - CNRS UMR 8579
% contact: regis.cottereau@ecp.fr

% initialization
Nm = length(model);
sol = cell(Nm,1);

% output the solution field in a format appropriate to each model
for i1 = 1:Nm
    
    % extract solution of the current model
    ui = full( u( (opt.iK(i1)+1):opt.iK(i1+1), : ) );

    % transformation depending on the model at hand
    switch model{i1}.code
        
        % ACOUSTIC - HOMEFE (DETERMINISTIC AND STOCHASTIC)
        case 'HomeFE'
            Nmc = size(u,2);
            Ndof = size(model{i1}.HomeFE.mesh.X,1);
            sol{i1} = zeros(Ndof,Nmc);
            sol{i1}(model{i1}.carl2Model,:) = ui(1:model{i1}.mesh.Nn);
            
        % ELASTIC - FE2D (DETERMINISTIC AND STOCHASTIC)
        case 'FE2D'
            Nmc = size(u,2);
            Ndof = size(model{i1}.FE2D.X,1);
            sol{i1} = zeros(Ndof,Nmc,2);
            sol{i1}(model{i1}.carl2Model,:,1) = ui(2*(1:model{i1}.mesh.Nn)-1,:);
            sol{i1}(model{i1}.carl2Model,:,2) = ui(2*(1:model{i1}.mesh.Nn),:);
            sol{i1} = permute( sol{i1}, [1 3 2] );
            
        % COMSOL
        case 'Comsol'
            error('not implemented yet: output in COMSOL format')
            
        % TIMOSCHENKO BEAM - BEAM
        case 'Beam'
            Ndof = size(model{i1}.Beam.X,1);
            sol{i1} = reshape( ui( 1:3*Ndof, 1 ), Ndof, 3 );
            
        % unknown case
        otherwise
            error('this external code is not supported')
            
    end
end

