import {useEffect, useState} from 'react'

function Square({value, onSquareClick}) {

  return (
    <button
      className="square"
      onClick={onSquareClick}
    >
      {value}
    </button>
  );
}


export default function Board() {
  const [squares, setSquares] = useState(Array(9).fill(null))
  const [xIsNext, setXIsNext] = useState(true);

  const [counter, setCounter] = useState(0);
  const [isClicked, setClicked] = useState(false)
  const [selectedSquare, setSelectedSquare] = useState(0);

  useEffect(() => {
  }, [squares]); 


  function checkMove(i)
  {
    let moves = [];
    switch(i){
      case 0:
      case 3:
      case 6:
        moves.push(i+1);
        moves.push(i+3);
        moves.push(i+4);
        moves.push(i-2);
        moves.push(i-3);
        break;
      case 1:
      case 4:
      case 7:
        moves.push(i+1);
        moves.push(i-1);
        moves.push(i+3);
        moves.push(i-3);
        moves.push(i-2);
        moves.push(i+2);
        moves.push(i+4);
        moves.push(i-4);
        break;
      case 2:
      case 5:
      case 8:
        moves.push(i-1);
        moves.push(i+3);
        moves.push(i-4);
        moves.push(i+2);
        moves.push(i-3);
        break;
      
    }
    return moves;
  }
  
  function handleClick(i) 
  {
    const nextSquares = squares.slice();
    let x = xIsNext ? "X" : "O";

    if(counter < 3){if (squares[i] || winner){return;}}
    else if ((squares[i] && squares[i] != x) || winner){return;}
    if(counter < 3)
    {
      if (xIsNext) {
        nextSquares[i] = "X";
      } else {
        nextSquares[i] = "O";
        setCounter(counter+1)
      }
    } else {
      if(nextSquares[i] == x && !isClicked)
      {
        setClicked(true);
        setSelectedSquare(i);
        return;
      } else if (!squares[i] && isClicked){
        const validMoves = checkMove(selectedSquare);
        if(!(validMoves.includes(i))){return;}
        if(nextSquares[4] && nextSquares[4] === x)
        {
          nextSquares[selectedSquare] = null;
          nextSquares[i] = x;
          if(!calculateWinner(nextSquares) && nextSquares[4])
          {
            nextSquares[selectedSquare] = x;
            nextSquares[i] = null;
            setClicked(false);
            return;
          }
          setClicked(false);
        } else {
          nextSquares[selectedSquare] = null;
          nextSquares[i] = x;
          setClicked(false);
        }
      } else {
        return;
      }
    }
    setSquares(nextSquares);
    setXIsNext(!xIsNext);
  }

  const winner = calculateWinner(squares);
  let status;
  if (winner) {
    status = "Winner: " + winner;
  } else {
    status = "Next player: " + (xIsNext ? "X" : "O");
  }

  return (
    <>
    <div className='status'>{status}</div>
    <div className="board-row">
      <Square value={squares[0]} onSquareClick={()=>{handleClick(0)}}/>
      <Square value={squares[1]} onSquareClick={()=>{handleClick(1)}}/>
      <Square value={squares[2]} onSquareClick={()=>{handleClick(2)}}/>
    </div>
      
    <div className="board-row">
      <Square value={squares[3]} onSquareClick={()=>{handleClick(3)}}/>
      <Square value={squares[4]} onSquareClick={()=>{handleClick(4)}}/>
      <Square value={squares[5]} onSquareClick={()=>{handleClick(5)}}/>
    </div>

    <div className="board-row">
      <Square value={squares[6]} onSquareClick={()=>{handleClick(6)}}/>
      <Square value={squares[7]} onSquareClick={()=>{handleClick(7)}}/>
      <Square value={squares[8]} onSquareClick={()=>{handleClick(8)}}/>
    </div>
    </>
  );
  
}

function calculateWinner(squares) {
  const lines = [
    [0, 1, 2],
    [3, 4, 5],
    [6, 7, 8],
    [0, 3, 6],
    [1, 4, 7],
    [2, 5, 8],
    [0, 4, 8],
    [2, 4, 6]
  ];
  for (let i = 0; i < lines.length; i++) {
    const [a, b, c] = lines[i];
    if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
      return squares[a];
    }
  }
  return null;
}
