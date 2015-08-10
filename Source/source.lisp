( let A ( ) ( 5 )
  letrec Fib ( int )
  ( let A ( ) ( 1 )
    let B ( ) ( 2 )
    letrec ABC ( ) ( + 1 2 )

    if ( | ( = int A ) ( = int B ) ) ( 1 ) ( + ( Fib ( - int A ) ) ( Fib ( - int B ) ) ) )

  Fib ( Fib ( 5 ) ) )
