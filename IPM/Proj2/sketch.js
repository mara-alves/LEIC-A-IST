// Bakeoff #2 - Seleção de Alvos e Fatores Humanos
// IPM 2020-21, Semestre 2
// Entrega: até dia 7 de Maio às 23h59 através do Fenix
// Bake-off: durante os laboratórios da semana de 3 de Maio

// p5.js reference: https://p5js.org/reference/

// Database (CHANGE THESE!)
const GROUP_NUMBER   = 52;      // Add your group number here as an integer (e.g., 2, 3)
const BAKE_OFF_DAY   = true;  // Set to 'true' before sharing during the simulation and bake-off days

// Target and grid properties (DO NOT CHANGE!)
let PPI, PPCM;
let TARGET_SIZE;
let TARGET_PADDING, MARGIN, LEFT_PADDING, TOP_PADDING;
let continue_button;

// Metrics
let testStartTime, testEndTime;// time between the start and end of one attempt (48 trials)
let hits 			 = 0;      // number of successful selections
let misses 			 = 0;      // number of missed selections (used to calculate accuracy)
let database;                  // Firebase DB  

// Study control parameters
let draw_targets     = false;  // used to control what to show in draw()
let trials 			 = [];     // contains the order of targets that activate in the test
let current_trial    = 0;      // the current trial number (indexes into trials array above)
let attempt          = 0;      // users complete each test twice to account for practice (attemps 0 and 1)
let fitts_IDs        = [];     // add the Fitts ID for each selection here (-1 when there is a miss)

let previousX;
let previousY;
let sound;
let missed;

// Target class (position and width)
class Target
{
  constructor(x, y, w)
  {
    this.x = x;
    this.y = y;
    this.w = w;
  }
}

function preload()
{
  sound = loadSound('assets/sound.mp3');
  missed = loadSound('assets/missed.mp3');
}

// Runs once at the start
function setup()
{
  cursor('assets/cursor2.png', 16, 16);
  previousX = mouseX;
  previousY = mouseY;
  createCanvas(700, 500);    // window size in px before we go into fullScreen()
  frameRate(60);             // frame rate (DO NOT CHANGE!)
  
  randomizeTrials();         // randomize the trial order at the start of execution

  drawUserIDScreen();        // draws the user input screen (student number and display size)
  
  noStroke();
  fill(color(255,255,255));
  textAlign(LEFT);
  
  textFont("Arial", 16);
  
  text("Instructions", width * 0.015, height * 0.32);
  
  textFont("Arial", 14);
  
  text("Current Target: ", width * 0.015, height * 0.40);
  push();
  noStroke();
  fill(color(230, 83, 89));
  circle(width * 0.015 + 120, height * 0.40, 40);
  fill(color(234, 132, 135));
  drawStar(width * 0.015 + 120, height * 0.40, 8, 20, 5);
  pop();

  text("Double Click: ", width * 0.015, height * 0.51);
  push();
  stroke(255);
  strokeWeight(3);
  fill(color(230, 83, 89));
  circle(width * 0.015 + 120, height * 0.51, 40);
  noStroke();
  fill(color(234, 132, 135));
  drawStar(width * 0.015 + 120, height * 0.51, 8, 20, 5);
  fill(255);
  drawStar(width * 0.015 + 120, height * 0.51, 4, 10, 5);
  pop();
  
  text("Next Target: ", width * 0.015, height * 0.62);
  push();
  noStroke();
  fill(color(188, 188, 188));
  circle(width * 0.015 + 120, height * 0.62, 40);
  fill(255);
  drawStar(width * 0.015 + 120, height * 0.62, 8, 20, 5);
  pop();
  
  textFont("Arial", 18);     // font size for the majority of the text
}

// Runs every frame and redraws the screen
function draw()
{
  if (draw_targets)
  {
    // The user is interacting with the 4x4 target grid
    background(color(34, 45, 52));        // sets background to black
    
    // Print trial count at the top left-corner of the canvas
    fill(color(255,255,255));
    textAlign(LEFT);
    text("Trial " + (current_trial + 1) + " of " + trials.length, 50, 20);
    
    // Draw all 16 targets
	for (var i = 0; i < 16; i++) drawTarget(i);
    
    // Draw arrows
    if (trials[current_trial + 1] !== trials[current_trial] && current_trial !== 47)
      drawArrow(trials[current_trial], trials[current_trial+1], 255);
    if (trials[current_trial-1] !== trials[current_trial] && current_trial !== 0)
      drawArrow(trials[current_trial-1], trials[current_trial], color(234, 132, 135));
    if (trials[current_trial + 1] == trials[current_trial])
      drawTriangle(trials[current_trial], trials[current_trial+1], 255);
    if (trials[current_trial - 1] == trials[current_trial])
      drawTriangle(trials[current_trial-1], trials[current_trial], color(234, 132, 135));
  }
}

// Print and save results at the end of 48 trials
function printAndSavePerformance()
{    
  // DO NOT CHANGE THESE! 
  let accuracy			= parseFloat(hits * 100) / parseFloat(hits + misses);
  let test_time         = (testEndTime - testStartTime) / 1000;
  let time_per_target   = nf((test_time) / parseFloat(hits + misses), 0, 3);
  let penalty           = constrain((((parseFloat(95) - (parseFloat(hits * 100) / parseFloat(hits + misses))) * 0.2)), 0, 100);
  let target_w_penalty	= nf(((test_time) / parseFloat(hits + misses) + penalty), 0, 3);
  let timestamp         = day() + "/" + month() + "/" + year() + "  " + hour() + ":" + minute() + ":" + second();
  
  background(color(0,0,0));   // clears screen
  fill(color(255,255,255));   // set text fill color to white
  text(timestamp, 10, 20);    // display time on screen (top-left corner)
  
  textAlign(CENTER);
  text("Attempt " + (attempt + 1) + " out of 2 completed!", width/2, 60); 
  text("Hits: " + hits, width/2, 100);
  text("Misses: " + misses, width/2, 120);
  text("Accuracy: " + accuracy + "%", width/2, 140);
  text("Total time taken: " + test_time + "s", width/2, 160);
  text("Average time per target: " + time_per_target + "s", width/2, 180);
  text("Average time for each target (+ penalty): " + target_w_penalty + "s", width/2, 220);
  
  // Print Fitts IDS (one per target, -1 if failed selection)
  let i = 1;
  let line1 = 280;
  textAlign(CENTER);
  text("Fitts Index of Performance", width/2, 260);
  while(i<25){
    textAlign(CENTER);
    if(fitts_IDs[i-1] != -1){
      text("Target "+ i+": "+ fitts_IDs[i-1], width/3, line1);
    }
    else{
      text("Target "+ i+": MISSED", width/3, line1);
    }
    line1 += 20;
    i++;
  }
  
  let line2 = 280;
  while(i<49){
    textAlign(CENTER);
    if(fitts_IDs[i-1] != -1){
      text("Target "+ i+": "+ fitts_IDs[i-1], 2*width/3, line2);
    }
    else{
      text("Target "+ i+": MISSED", 2*width/3, line2);
    }
    line2 += 20;
    i++;
  }

  // Saves results (DO NOT CHANGE!)
  let attempt_data = 
  {
        project_from:       GROUP_NUMBER,
        assessed_by:        student_ID,
        test_completed_by:  timestamp,
        attempt:            attempt,
        hits:               hits,
        misses:             misses,
        accuracy:           accuracy,
        attempt_duration:   test_time,
        time_per_target:    time_per_target,
        target_w_penalty:   target_w_penalty,
        fitts_IDs:          fitts_IDs
  }
  
  // Send data to DB (DO NOT CHANGE!)
  if (BAKE_OFF_DAY)
  {
    // Access the Firebase DB
    if (attempt === 0)
    {
      firebase.initializeApp(firebaseConfig);
      database = firebase.database();
    }
    
    // Add user performance results
    let db_ref = database.ref('G' + GROUP_NUMBER);
    db_ref.push(attempt_data);
  }
}

// Mouse button was pressed - lets test to see if hit was in the correct target
function mousePressed() 
{
  // Only look for mouse releases during the actual test
  // (i.e., during target selections)
  if (draw_targets)
  {
    // Get the location and size of the target the user should be trying to select
    let target = getTargetBounds(trials[current_trial]);   
    
    // Check to see if the mouse cursor is inside the target bounds,
    // increasing either the 'hits' or 'misses' counters
    if (dist(target.x, target.y, mouseX, mouseY) < target.w/2)
    {
      sound.play();
      hits++;
      fitts_IDs[current_trial] = round(Math.log2(dist(previousX, previousY, target.x, target.y) / TARGET_SIZE + 1) , 3);
    }
    else 
    {
      missed.setVolume(0.3);
      missed.play();
      misses++;
      fitts_IDs[current_trial] = -1;
     }
    
    current_trial++;                 // Move on to the next trial/target
    previousX = mouseX;
    previousY = mouseY;
    
    // Check if the user has completed all 48 trials
    if (current_trial === trials.length)
    {
      testEndTime = millis();
      draw_targets = false;          // Stop showing targets and the user performance results
      printAndSavePerformance();     // Print the user's results on-screen and send these to the DB
      attempt++;                      
      
      // If there's an attempt to go create a button to start this
      if (attempt < 2)
      {
        continue_button = createButton('START 2ND ATTEMPT');
        continue_button.mouseReleased(continueTest);
        continue_button.position(width/2 - continue_button.size().width/2, height/2 - continue_button.size().height/2);
      }
    } 
  }
}

// Draw an arrow
function drawArrow(from, to, color) {
  let target = getTargetBounds(from);
  let next = getTargetBounds(to);
  
  var angle = atan2(target.y - next.y, target.x - next.x); //gets the angle of the line
  var distance = dist(target.x, target.y, next.x, next.y); //gets the distance of the line
  
  // make the arrow pointline
  push();
  translate((target.x + next.x)/2, (target.y + next.y)/2); //translates to the middle of the targets' vertices
  rotate(angle); //rotates the arrow line
  stroke(color);
  strokeWeight(10);
  fill(color);
  line(-distance/2 + TARGET_SIZE/2 + 20, 0, distance/2 - TARGET_SIZE/2 - 5, 0); //draw a line beetween the vertices
  pop();
  
  // make the arrow point
  push();
  noStroke();
  translate(next.x, next.y); //translates to the destination vertex
  rotate(angle-HALF_PI); //rotates the arrow point
  fill(color);
  triangle(-TARGET_SIZE*0.2, TARGET_SIZE*0.8, TARGET_SIZE*0.2, TARGET_SIZE*0.8, 0, TARGET_SIZE/2); //draws the arrow point as a triangle
  pop();
}

function drawTriangle(from, to, color){
  let target = getTargetBounds(from);
  let next = getTargetBounds(to);
  
  var angle = atan2(target.y - next.y, target.x - next.x); //gets the angle of the line
  // make the arrow point
  push();
  noStroke();
  translate(next.x, next.y); //translates to the destination vertex
  rotate(angle-HALF_PI); //rotates the arrow point
  fill(color);
  triangle(-TARGET_SIZE*0.2, TARGET_SIZE*0.8, TARGET_SIZE*0.2, TARGET_SIZE*0.8, 0, TARGET_SIZE/2); //draws the arrow point as a triangle
  pop();
}

function drawStar(x, y, radius1, radius2, npoints)
{
  let angle = TWO_PI / npoints;
  let halfAngle = angle / 2.0;
  beginShape();
  for (let a = 0; a < TWO_PI; a += angle)
  {
    let sx = x + cos(a) * radius2;
    let sy = y + sin(a) * radius2;
    vertex(sx, sy);
    sx = x + cos(a + halfAngle) * radius1;
    sy = y + sin(a + halfAngle) * radius1;
    vertex(sx, sy);
  }
  endShape(CLOSE);
}

// Draw target on-screen
function drawTarget(i)
{ 
  // Get the location and size for target (i)
  let target = getTargetBounds(i);
  
  // Check whether this target is the target the user should be trying to select
  if (trials[current_trial] === i) 
  { 
    if (trials[current_trial + 1] == trials[current_trial])
    {
      stroke(255);
      strokeWeight(5);
    }
    else noStroke(); 
    
    if (dist(target.x, target.y, mouseX, mouseY) < target.w/2) fill(color(234, 132, 135));
    else fill(color(230, 83, 89));
    
    circle(target.x, target.y, target.w);
    
    push();
    rectMode(CENTER);
    translate(target.x, target.y);
    rotate(frameCount / 100.0);
    noStroke();
    
    if (dist(target.x, target.y, mouseX, mouseY) < target.w/2) fill(230, 83, 89);
    else fill(color(234, 132, 135));
    
    drawStar(0, 0, target.w * 0.2, target.w * 0.5, 5);
    
    if (trials[current_trial + 1] == trials[current_trial]) {
      fill(255);
      drawStar(0, 0, target.w * 0.1, target.w * 0.25, 5);
    }
    
    pop();
  }
  else if (trials[current_trial + 1] === i)
  {
    noStroke();
    fill(color(188, 188, 188));
    circle(target.x, target.y, target.w);
    fill(255);
    drawStar(target.x, target.y, target.w * 0.2, target.w * 0.5, 5);
  }
  // Other circles
  else 
  { 
    noStroke();          
    fill(color(188, 188, 188));                 
    circle(target.x, target.y, target.w);
  }
}


// Returns the location and size of a given target
function getTargetBounds(i)
{
  var x = parseInt(LEFT_PADDING) + parseInt((i % 4) * (TARGET_SIZE + TARGET_PADDING) + MARGIN);
  var y = parseInt(TOP_PADDING) + parseInt(Math.floor(i / 4) * (TARGET_SIZE + TARGET_PADDING) + MARGIN);

  return new Target(x, y, TARGET_SIZE);
}

// Evoked after the user starts its second (and last) attempt
function continueTest()
{
  // Re-randomize the trial order
  shuffle(trials, true);
  current_trial = 0;
  print("trial order: " + trials);
  
  // Resets performance variables
  hits = 0;
  misses = 0;
  fitts_IDs = [];
  
  continue_button.remove();
  
  // Shows the targets again
  draw_targets = true;
  testStartTime = millis();  
}

// Is invoked when the canvas is resized (e.g., when we go fullscreen)
function windowResized() 
{
  resizeCanvas(windowWidth, windowHeight);
    
  let display    = new Display({ diagonal: display_size }, window.screen);

  // DO NOT CHANGE THESE!
  PPI            = display.ppi;                        // calculates pixels per inch
  PPCM           = PPI / 2.54;                         // calculates pixels per cm
  TARGET_SIZE    = 1.5 * PPCM;                         // sets the target size in cm, i.e, 1.5cm
  TARGET_PADDING = 1.5 * PPCM;                         // sets the padding around the targets in cm
  MARGIN         = 1.5 * PPCM;                         // sets the margin around the targets in cm

  // Sets the margin of the grid of targets to the left of the canvas (DO NOT CHANGE!)
  LEFT_PADDING   = width/2 - TARGET_SIZE - 1.5 * TARGET_PADDING - 1.5 * MARGIN;        
  
  // Sets the margin of the grid of targets to the top of the canvas (DO NOT CHANGE!)
  TOP_PADDING    = height/2 - TARGET_SIZE - 1.5 * TARGET_PADDING - 1.5 * MARGIN;
  
  // Starts drawing targets immediately after we go fullscreen
  draw_targets = true;
}