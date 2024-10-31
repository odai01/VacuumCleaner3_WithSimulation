import pygame
import json
import math

# Load data from the JSON files (steps history and the house matrix)
with open('steps_history.json') as f:
    steps_history = json.load(f)
with open('initial_house.json') as f:
    house_data = json.load(f)

houses = house_data['houses']

# Initialize Pygame
pygame.init()
pygame.font.init()

# Display dimensions
block_size = 50  # If we want to enlarge the house we can increase block size
margin = 20
current_house_index = 0
# Colors we will use
MAROON = (128, 0, 0)
LIGHTYELLOW = (216, 191, 216)  # it is light purple not light yellow
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GOLD = (255, 215, 0)
WALL_COLOR = (0, 0, 128)
GREEN = (107, 142, 35)
RED = (255, 99, 71)
BatteryBar = (50, 205, 50)
StepsBar = (64, 224, 208)
DIVIDER_COLOR = BLACK

# Function to calculate gray color based on dirt level
def get_dirt_color(level):
    if level < 0 or level > 9:
        return WHITE
    gray_value = 255 - (level * 25)
    return (gray_value, gray_value, gray_value)

# Function to draw the house grid and the robot
def draw_grid(surface, house, steps, start_x, start_y, step_index, docking_station, charging):
    rows = len(house)
    cols = len(house[0])
    for i in range(rows):
        for j in range(cols):
            if house[i][j] == -1:
                color = WALL_COLOR
            else:
                color = get_dirt_color(house[i][j])
            rect = pygame.Rect(start_x + j * block_size, start_y + i * block_size, block_size, block_size)
            pygame.draw.rect(surface, color, rect)
            if house[i][j] > 0:
                font = pygame.font.SysFont(None, 24)
                text = font.render(str(house[i][j]), True, BLACK)
                surface.blit(text, (start_x + j * block_size + block_size // 2 - text.get_width() // 2,
                                    start_y + i * block_size + block_size // 2 - text.get_height() // 2))
            if (i, j) == (docking_station['x'], docking_station['y']):
                draw_docking_station_symbol(surface, start_x + j * block_size, start_y + i * block_size, charging)

    # Draw the vacuum cleaner
    if step_index < len(steps):
        x, y = steps[step_index]['x'], steps[step_index]['y']
        vacuum_center = (start_x + y * block_size + block_size // 2,
                         start_y + x * block_size + block_size // 2)
        pygame.draw.circle(surface, GOLD, vacuum_center, block_size // 4)
        # Decrease the dirt level at the vacuum's position (simulate cleaning)
        if house[x][y] > 0:
            house[x][y] -= 1

# Function to draw the docking station symbol: the Plus sign
def draw_docking_station_symbol(surface, x, y, charging):
    color = GOLD if charging else BLACK
    pygame.draw.line(surface, color, (x + 7, y + 5), (x + 14, y + 5), 2)
    pygame.draw.line(surface, color, (x + 10, y + 2), (x + 10, y + 9), 2)

# Function to draw the legend
def draw_legend(surface, start_x, start_y):
    font = pygame.font.SysFont(None, 20)  # Slightly smaller font size
    legend_items = [
        ("Wall", WALL_COLOR),
        ("Vacuum Cleaner", GOLD),
        ("Dirt Levels", BLACK),
        ("Docking Station", WHITE)  # Adding Docking Station legend here
    ]
    x, y = start_x + margin, start_y - 20  # Raised up by 20 pixels
    for text, color in legend_items:
        if text == "Vacuum Cleaner":
            pygame.draw.circle(surface, color, (x + block_size // 3, y + block_size // 3), block_size // 4)  # Smaller circle
            legend_text = font.render(text, True, BLACK)
            surface.blit(legend_text, (x + block_size + 10, y + block_size // 3 - legend_text.get_height() // 2))
        elif text == "Dirt Levels":
            for level in range(10):
                dirt_color = get_dirt_color(level)
                pygame.draw.rect(surface, dirt_color, (x + level * (block_size // 12), y, block_size // 9, block_size // 3))  # Smaller rectangles
            legend_text = font.render(text, True, BLACK)
            surface.blit(legend_text, (x + block_size + 10, y + block_size // 3 - legend_text.get_height() // 2))
        elif text == "Docking Station":
            pygame.draw.rect(surface, color, (x, y, block_size // 1.2, block_size // 1.2))  # Smaller docking station
            draw_docking_station_symbol(surface, x, y, False)  # Initially, not charging
            legend_text = font.render(text, True, BLACK)
            surface.blit(legend_text, (x + block_size + 10, y + block_size // 3 - legend_text.get_height() // 2))
        else:
            pygame.draw.rect(surface, color, (x, y, block_size // 1.2, block_size // 1.2))  # Smaller squares
            legend_text = font.render(text, True, BLACK)
            surface.blit(legend_text, (x + block_size + 10, y + block_size // 3 - legend_text.get_height() // 2))
        y += block_size // 2 + 15  # Adjust spacing between legend items
 


# Function to draw status
def draw_status(surface, battery, max_battery, steps, max_steps, battery_x, steps_x):
    if battery < 0 or steps > max_steps:
        return
    
    font = pygame.font.SysFont(None, 30)
    
    # Battery status on the left top corner
    battery_status_text = f"Battery: {battery}/{max_battery}"
    battery_status_surface = font.render(battery_status_text, True, BLACK)
    battery_y = margin
    surface.blit(battery_status_surface, (battery_x, battery_y))
    
    # Battery bar (battery icon style)
    bar_width = 200
    bar_height = 20
    battery_percentage = battery / max_battery
    battery_bar_length = int(bar_width * battery_percentage)
    
    battery_outer_rect = pygame.Rect(battery_x, battery_y + 30, bar_width + 10, bar_height + 10)
    battery_inner_rect = pygame.Rect(battery_x + 5, battery_y + 35, bar_width, bar_height)
    battery_fill_rect = pygame.Rect(battery_x + 5, battery_y + 35, battery_bar_length, bar_height)
    battery_tip_rect = pygame.Rect(battery_x + bar_width + 10, battery_y + 35 + bar_height // 4, 5, bar_height // 2)
    
    pygame.draw.rect(surface, BLACK, battery_outer_rect, 2)  # Outer border
    pygame.draw.rect(surface, BLACK, battery_tip_rect)  # Battery tip
    pygame.draw.rect(surface, BatteryBar, battery_fill_rect)  # Battery fill
    pygame.draw.rect(surface, BLACK, battery_inner_rect, 2)  # Inner border
    
    # Steps status on the right top corner
    steps_status_text = f"Steps: {steps}/{max_steps}"
    steps_status_surface = font.render(steps_status_text, True, BLACK)
    steps_y = margin
    surface.blit(steps_status_surface, (steps_x, steps_y))

    # Steps bar (gradient fill style)
    bar_width = 200
    steps_bar_length = bar_width
    steps_bar_height = 20
    steps_bar_x = steps_x - bar_width + steps_status_surface.get_width()
    steps_bar_y = steps_y + 30

    steps_percentage = steps / max_steps
    steps_fill_length = int(steps_bar_length * steps_percentage)

    # Draw the empty steps bar
    pygame.draw.rect(surface, BLACK, (steps_bar_x, steps_bar_y, steps_bar_length, steps_bar_height), 2)

    # Draw the filled part of the steps bar with gradient
    for i in range(steps_fill_length):
        gradient_color = (
            255,  # Red remains constant
            int(0 + 255 * (1 - i / steps_fill_length)),  # Green increases from 0 to 255
            int(255 * (1 - i / steps_fill_length))  # Blue decreases from 255 to 0
        )

        pygame.draw.rect(surface, gradient_color, (steps_bar_x + i, steps_bar_y, 1, steps_bar_height))

def run_simulation(house_data, dfs_steps, spiral_steps, dfs_score, spiral_score):
    global window, screen_width, screen_height

    # Initialize Pygame again for each house
    pygame.init()
    pygame.font.init()

    running = True
    paused = False
    step_index_dfs = 0
    step_index_spiral = 0
    clock = pygame.time.Clock()

    house = house_data['house']
    maxSteps = house_data['maxSteps']
    maxBattery = house_data['maxBattery']
    battery_state_dfs = maxBattery
    battery_state_spiral = maxBattery

    docking_station = {'x': dfs_steps[0]['x'], 'y': dfs_steps[0]['y']}
    
    rows = len(house)
    cols = len(house[0])

    # Increase window size for better display
    screen_width = 1600
    screen_height = 800

    window = pygame.display.set_mode((screen_width, screen_height + 100))

    dfs_surface = pygame.Surface((screen_width // 2, screen_height))
    spiral_surface = pygame.Surface((screen_width // 2, screen_height))

    dfs_finished = False
    spiral_finished = False

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN:
                mouse_pos = pygame.mouse.get_pos()
                if screen_width // 2 - 60 <= mouse_pos[0] <= screen_width // 2 + 60 and screen_height + 40 <= mouse_pos[1] <= screen_height + 80:
                    running = False  # Continue button clicked

        window.fill(LIGHTYELLOW)
        dfs_surface.fill(LIGHTYELLOW)
        spiral_surface.fill(LIGHTYELLOW)

        # Calculate positions
        house_center_x = dfs_surface.get_width() // 2 - (cols * block_size) // 2
        house_center_y = dfs_surface.get_height() // 2 - (rows * block_size) // 2
        battery_x = margin
        steps_x = dfs_surface.get_width() - margin - 200  # Offset for step bar
        legend_y = screen_height - 150

        # Determine if vacuum is charging and manage steps for both algorithms
        if not dfs_finished:
            charging_dfs = (docking_station['x'] == dfs_steps[step_index_dfs]['x'] and docking_station['y'] == dfs_steps[step_index_dfs]['y'])
            if charging_dfs:
                battery_state_dfs = min(maxBattery, battery_state_dfs + maxBattery / 20.0)
            else:
                battery_state_dfs -= 1

            if step_index_dfs < len(dfs_steps) - 1:
                step_index_dfs += 1
            else:
                dfs_finished = True

        if not spiral_finished:
            charging_spiral = (docking_station['x'] == spiral_steps[step_index_spiral]['x'] and docking_station['y'] == spiral_steps[step_index_spiral]['y'])
            if charging_spiral:
                battery_state_spiral = min(maxBattery, battery_state_spiral + maxBattery / 20.0)
            else:
                battery_state_spiral -= 1

            if step_index_spiral < len(spiral_steps) - 1:
                step_index_spiral += 1
            else:
                spiral_finished = True

        # Draw grids for both algorithms, including the robot
        draw_grid(dfs_surface, house, dfs_steps, house_center_x, house_center_y, step_index_dfs, docking_station, charging_dfs)
        draw_grid(spiral_surface, house, spiral_steps, house_center_x, house_center_y, step_index_spiral, docking_station, charging_spiral)

        # Draw the legends and status bars for both algorithms
        draw_legend(dfs_surface, margin, legend_y)
        draw_legend(spiral_surface, margin, legend_y)

        draw_status(dfs_surface, battery_state_dfs, maxBattery, step_index_dfs, maxSteps, battery_x, steps_x)
        draw_status(spiral_surface, battery_state_spiral, maxBattery, step_index_spiral, maxSteps, battery_x, steps_x)

        # Draw the divider between the two halves
        pygame.draw.line(window, DIVIDER_COLOR, (screen_width // 2, 0), (screen_width // 2, screen_height), 4)

        # Blit the surfaces onto the main window
        window.blit(dfs_surface, (0, 0))
        window.blit(spiral_surface, (screen_width // 2, 0))

        # Display scores once the algorithms finish
        if dfs_finished:
            font = pygame.font.SysFont(None, 36)
            dfs_score_text = f"DFS Score: {dfs_score}"
            dfs_score_surface = font.render(dfs_score_text, True, RED)
            window.blit(dfs_score_surface, (300, 20))

        if spiral_finished:
            font = pygame.font.SysFont(None, 36)
            spiral_score_text = f"Spiral Score: {spiral_score}"
            spiral_score_surface = font.render(spiral_score_text, True, RED)
            window.blit(spiral_score_surface, (screen_width // 2 + 300, 20))

        # Determine winner once both algorithms have finished
        if dfs_finished and spiral_finished:
            font = pygame.font.SysFont(None, 48)
            if dfs_score < spiral_score:
                winner_text = f"DFS Algorithm wins cleaning the house {house_data['houseName']} with score: {dfs_score}"
            else:
                winner_text = f"Spiral Algorithm wins cleaning the house {house_data['houseName']} with score: {spiral_score}"
            winner_surface = font.render(winner_text, True, BLACK)

            # Adjust the y-coordinate to move the text lower on the screen
            y_position = screen_height - winner_surface.get_height() - 600  # Change this value to move text lower

            window.blit(winner_surface, (screen_width // 2 - winner_surface.get_width() // 2, y_position))

        # Draw the Continue button
        font = pygame.font.SysFont(None, 36)
        button_rect = pygame.Rect(screen_width // 2 - 60, screen_height + 40, 120, 40)
        pygame.draw.rect(window, GREEN, button_rect)
        button_text = font.render("Continue", True, BLACK)
        window.blit(button_text, (button_rect.x + (button_rect.width - button_text.get_width()) // 2,
                                button_rect.y + (button_rect.height - button_text.get_height()) // 2))
        pygame.display.flip()
        clock.tick(8)  # Control simulation speed

    pygame.quit()




def run_all_simulations():
    global current_house_index
    running = True

    while running and current_house_index < len(houses):
        house = houses[current_house_index]
        dfs_steps = [entry['steps'] for entry in steps_history if entry['algorithmName'] == 'DFSAlgorithm' and entry['houseName'] == house['houseName']]
        spiral_steps = [entry['steps'] for entry in steps_history if entry['algorithmName'] == 'SpiralCleaningAlgorithm' and entry['houseName'] == house['houseName']]
        
        # Extract the scores from the JSON data
        dfs_score = next((entry['score'] for entry in steps_history if entry['algorithmName'] == 'DFSAlgorithm' and entry['houseName'] == house['houseName']), 0)
        spiral_score = next((entry['score'] for entry in steps_history if entry['algorithmName'] == 'SpiralCleaningAlgorithm' and entry['houseName'] == house['houseName']), 0)

        if dfs_steps and spiral_steps:
            run_simulation(house, dfs_steps[0], spiral_steps[0], dfs_score, spiral_score)
        
        current_house_index += 2

        if current_house_index >= len(houses):
            running = False
        else:
            pygame.quit()
            pygame.init()
            pygame.font.init()



run_all_simulations()