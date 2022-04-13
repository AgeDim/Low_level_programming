%ifndef current_node
    %define current_node 0
%endif 

%macro colon 2
    %if %0 = 2
        %ifid %2
             %2:
                dq current_node 
                %define current_node %2        
        %else 
            %error "the label must be an identifier" 
        %endif 
        %ifstr %1
            db %1, 0        
        %else
            %error "the key must be a string value" 
        %endif
    %else 
        %error "the macro must take 2 arguments"
    %endif
%endmacro
