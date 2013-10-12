# multiAgents.py
# --------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html

from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
  """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
  """


  def getAction(self, gameState):
    """
    You do not need to change this method, but you're welcome to.

    getAction chooses among the best options according to the evaluation function.

    Just like in the previous project, getAction takes a GameState and returns
    some Directions.X for some X in the set {North, South, West, East, Stop}
    """
    # Collect legal moves and successor states
    legalMoves = gameState.getLegalActions()

    # Choose one of the best actions
    scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
    chosenIndex = random.choice(bestIndices) # Pick randomly among the best

    "Add more of your code here if you want to"
    return legalMoves[chosenIndex]

  def evaluationFunction(self, currentGameState, action):
    """
    Design a better evaluation function here.

    The evaluation function takes in the current and proposed successor
    GameStates (pacman.py) and returns a number, where higher numbers are better.

    The code below extracts some useful information from the state, like the
    remaining food (newFood) and Pacman position after moving (newPos).
    newScaredTimes holds the number of moves that each ghost will remain
    scared because of Pacman having eaten a power pellet.

    Print out these variables to see what you're getting, then combine them
    to create a masterful evaluation function.
    """
    # Useful information you can extract from a GameState (pacman.py)
    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanPosition()
    newFood = successorGameState.getFood()
    newGhostStates = successorGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    "*** YOUR CODE HERE ***"
    totalSoFar = 0
    #ghost avoidance
    for ghostPos in currentGameState.getGhostPositions():
        if (manhattanDistance(newPos, ghostPos) <= 1):
            totalSoFar += -100
    
    #default
    totalSoFar += successorGameState.getScore()
    #less food is better!
    
    totalSoFar += -5 * successorGameState.getNumFood()
    #greed is good
    heightRange, widthRange = range(currentGameState.getWalls().height-2), range(currentGameState.getWalls().width-2)
    tempSet = set([])
    for X in widthRange:
        for Y in heightRange:
            if newFood[X+1][Y+1]:
                tempSet.add(manhattanDistance(newPos, (X+1,Y+1)))
    if tempSet != set([]):
        totalSoFar += 10.0 * (1.0/(min(tempSet) * 1.0))
    return totalSoFar
    #return successorGameState.getScore()

def scoreEvaluationFunction(currentGameState):
  """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
  """
  return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
  """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
  """

  def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
    self.index = 0 # Pacman is always agent index 0
    self.evaluationFunction = util.lookup(evalFn, globals())
    self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
  """
    Your minimax agent (question 2)
  """

  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth
      and self.evaluationFunction.

      Here are some method calls that might be useful when implementing minimax.

      gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

      Directions.STOP:
        The stop direction, which is always legal

      gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

      gameState.getNumAgents():
        Returns the total number of agents in the game
    """
    "*** YOUR CODE HERE ***"
    return self.miniMax(gameState)

  def miniMax(self, gameState):
    legalMoves = gameState.getLegalActions(self.index)
    scores = [self.minValue(gameState.generateSuccessor(self.index, action), self.index, gameState.getNumAgents(), self.depth) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [ind for ind in range(len(scores)) if scores[ind] == bestScore]
    chosenIndex = random.choice(bestIndices)
    return legalMoves[chosenIndex]
      
  def minValue(self, gameState, previousAgentIndex, numAgents, depth):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = 9999999999999
    currentAgent = previousAgentIndex + 1
    if currentAgent == (numAgents-1):
        nextAgent = 0
    else: 
        nextAgent = currentAgent+1
        
    for action in gameState.getLegalActions(currentAgent):
        if nextAgent == 0:
            val = min(val, self.maxValue(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth-1))
        else:
            val = min(val, self.minValue(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth))
    return val
      
  def maxValue(self, gameState, previousAgentIndex, numAgents, depth):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = -9999999999999
    pacmanIndex = 0
    for action in gameState.getLegalActions(pacmanIndex):
        val = max(val, self.minValue(gameState.generateSuccessor(pacmanIndex, action), pacmanIndex, numAgents, depth))
    return val
      
  def terminalTest(self, gameState, depth):
    if depth == 0:
        return True
    if gameState.isLose():
        return True
    if gameState.isWin():
        return True
    return False
      
class AlphaBetaAgent(MultiAgentSearchAgent):
  """
    Your minimax agent with alpha-beta pruning (question 3)
  """

  def getAction(self, gameState):
    """
      Returns the minimax action using self.depth and self.evaluationFunction
    """
    "*** YOUR CODE HERE ***"
    return self.miniMaxAB(gameState)

  def miniMaxAB(self, gameState):
    legalMoves = gameState.getLegalActions(self.index)
    alpha = -9999999999
    beta = 9999999999
    scores = [self.minValueAB(gameState.generateSuccessor(self.index, action), self.index, gameState.getNumAgents(), self.depth, alpha, beta) for action in legalMoves]
    bestScore = max(scores)
    print bestScore
    bestIndices = [ind for ind in range(len(scores)) if scores[ind] == bestScore]
    chosenIndex = random.choice(bestIndices)
    return legalMoves[chosenIndex]
    """
    legalMoves = gameState.getLegalActions(self.index)
    val = -9999999999
    alpha = -9999999999
    beta = 9999999999
    scores = []
    for action in legalMoves:
        val = max(val, self.minValueAB(gameState.generateSuccessor(self.index, action), self.index, gameState.getNumAgents(), self.depth, alpha, beta))
        scores.append(val)
        alpha = max(val, alpha)
    bestScore = val
    bestIndices = [ind for ind in range(len(scores)) if scores[ind] == bestScore]
    chosenIndex = random.choice(bestIndices)
    return legalMoves[chosenIndex]
    """
      
  def minValueAB(self, gameState, previousAgentIndex, numAgents, depth, alpha, beta):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = 9999999999
    beta2 = beta
    currentAgent = previousAgentIndex + 1
    if currentAgent == (numAgents-1):
        nextAgent = 0
    else: 
        nextAgent = currentAgent+1
        
    for action in gameState.getLegalActions(currentAgent):
        if nextAgent == 0:
            val = min(val, self.maxValueAB(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth-1, alpha, beta2))
        else:
            val = min(val, self.minValueAB(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth, alpha, beta2))
        if val <= alpha:
            return val
        beta2 = min(beta2, val)
    return val
      
  def maxValueAB(self, gameState, previousAgentIndex, numAgents, depth, alpha, beta):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = -9999999999
    alpha2 = alpha
    pacmanIndex = 0
    for action in gameState.getLegalActions(pacmanIndex):
        val = max(val, self.minValueAB(gameState.generateSuccessor(pacmanIndex, action), pacmanIndex, numAgents, depth, alpha2, beta))
        if val >= beta:
            return val
        alpha2 = max(alpha2, val)
    return val
      
  def terminalTest(self, gameState, depth):
    if depth == 0:
        return True
    if gameState.isLose():
        return True
    if gameState.isWin():
        return True
    return False

class ExpectimaxAgent(MultiAgentSearchAgent):
  """
    Your expectimax agent (question 4)
  """

  def getAction(self, gameState):
    """
      Returns the expectimax action using self.depth and self.evaluationFunction

      All ghosts should be modeled as choosing uniformly at random from their
      legal moves.
    """
    "*** YOUR CODE HERE ***"
    return self.expectiMax(gameState)

  def expectiMax(self, gameState):
    legalMoves = gameState.getLegalActions(self.index)
    scores = [self.minValue(gameState.generateSuccessor(self.index, action), self.index, gameState.getNumAgents(), self.depth) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [ind for ind in range(len(scores)) if scores[ind] == bestScore]
    chosenIndex = random.choice(bestIndices)
    return legalMoves[chosenIndex]
      
  def minValue(self, gameState, previousAgentIndex, numAgents, depth):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = 0.0
    currentAgent = previousAgentIndex + 1
    if currentAgent == (numAgents-1):
        nextAgent = 0
    else: 
        nextAgent = currentAgent+1
    LegalActions = gameState.getLegalActions(currentAgent)
    odds = len(LegalActions)
    for action in LegalActions:
        if nextAgent == 0:
            val += self.maxValue(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth-1)
        else:
            val += self.minValue(gameState.generateSuccessor(currentAgent, action), currentAgent, numAgents, depth)
    return (val / (odds * 1.0))
      
  def maxValue(self, gameState, previousAgentIndex, numAgents, depth):
    if self.terminalTest(gameState, depth):
        return self.evaluationFunction(gameState)
    val = -9999999999999
    pacmanIndex = 0
    for action in gameState.getLegalActions(pacmanIndex):
        val = max(val, self.minValue(gameState.generateSuccessor(pacmanIndex, action), pacmanIndex, numAgents, depth))
    return val
      
  def terminalTest(self, gameState, depth):
    if depth == 0:
        return True
    if gameState.isLose():
        return True
    if gameState.isWin():
        return True
    return False

def betterEvaluationFunction(currentGameState):
  """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
    Added the current score as a default.
    Tweaked this by considering various things such as remaining pelets, distance to nearest pelet, distance to nearest ghost,
    distance to power pelet, etc.
    Put a lot of emphasis on staying away from ghosts if they were not scared.
  """
  "*** YOUR CODE HERE ***"
  totalSoFar = 0
  #default
  totalSoFar += currentGameState.getScore()
  #more food is bad
  totalSoFar += -5 * currentGameState.getNumFood()
  #greed is good (more points for being nearer to nearest food)
  heightRange, widthRange = range(currentGameState.getWalls().height-2), range(currentGameState.getWalls().width-2)
  tempSet = set([])
  for X in widthRange:
      for Y in heightRange:
          if currentGameState.getFood()[X+1][Y+1]:
              tempSet.add(manhattanDistance(currentGameState.getPacmanPosition(), (X+1,Y+1)))
  if tempSet != set([]):
      totalSoFar += 10.0 * (1.0/(min(tempSet) * 1.0))
  #get capsules
  totalSoFar += -50 * len(currentGameState.getCapsules())
          
  #ghost avoidance ###############################################3need to increase +1 dist?
  for ghostPos in currentGameState.getGhostPositions():
      #print currentGameState.getGhostState(1)
      if (manhattanDistance(currentGameState.getPacmanPosition(), ghostPos) <= 2):
          totalSoFar += -200
            
  return totalSoFar


# Abbreviation
better = betterEvaluationFunction

class ContestAgent(MultiAgentSearchAgent):
  """
    Your agent for the mini-contest
  """

  def getAction(self, gameState):
    """
      Returns an action.  You can use any method you want and search to any depth you want.
      Just remember that the mini-contest is timed, so you have to trade off speed and computation.

      Ghosts don't behave randomly anymore, but they aren't perfect either -- they'll usually
      just make a beeline straight towards Pacman (or away from him if they're scared!)
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

