
Table table;

{

//P0 P1 P2 P3 F0 F1 F2 F3 A0 
  table = new Table();
  
  table.addColumn("codigo");
  table.addColumn("letra");
  table.addColumn("video");
  table.addColumn ("frecuencia");
  table.addColumn("nota");


  //SONIDOS
  TableRow newRow = table.addRow();
  newRow.setString("codigo", "10000000");
  newRow.setString("nota", "DO");
  newRow.setInt("frecuencia", 262); 
  
  newRow = table.addRow();
  newRow.setString("codigo", "11000000");
  newRow.setString("nota", "RE");
  newRow.setInt("frecuencia", 294 ); 

  newRow = table.addRow();
  newRow.setString("codigo", "11100000");
  newRow.setString("nota", "MI");
  newRow.setInt("frecuencia", 330 );   

  newRow = table.addRow();
  newRow.setString("codigo", "00100000");
  newRow.setString("nota", "FA");
  newRow.setInt("frecuencia", 349 ); 

  newRow = table.addRow();
  newRow.setString("codigo", "01000000");
  newRow.setString("nota", "SOL");
  newRow.setInt("frecuencia", 392 );     
  
  newRow = table.addRow();
  newRow.setString("codigo", "01100000");
  newRow.setString("nota", "LA");
  newRow.setInt("frecuencia", 440 ); 

 newRow = table.addRow();
  newRow.setString("codigo", "01110000");
  newRow.setString("nota", "SI");
  newRow.setInt("frecuencia", 494 );   
  
  newRow = table.addRow();
  newRow.setString("codigo", "10100000");
  newRow.setString("nota", "DO2");
  newRow.setInt("frecuencia", 523 );     
 
}
