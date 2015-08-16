( 
    letrec rec ( _1 ) 
    (
        if ( != _1 1 ) ( rec ( - _1 1 ) ) ( 1 ) 
    )
    rec 2   
)