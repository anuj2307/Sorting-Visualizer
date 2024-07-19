#include <SFML/Graphics.hpp>
#include <vector>
#include <chrono>
#include <thread>
#include <stack>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BAR_WIDTH = 10;
const int NUM_BARS = WINDOW_WIDTH / BAR_WIDTH;
const int DELAY = 20;  

enum SortType { None, Bubble, Selection, Merge };

struct MergeSortState {
    int left;
    int right;
    int mid;
    int step;
    std::vector<int> leftArray;
    std::vector<int> rightArray;
    int leftIndex;
    int rightIndex;
    int mainIndex;

    MergeSortState(int l, int r, int m)
        : left(l), right(r), mid(m), step(0),
          leftIndex(0), rightIndex(0), mainIndex(l) {}
};

// Bubble Sort Step
void bubbleSortStep(std::vector<int> &array, int &step) {
    for (int j = 0; j < array.size() - step - 1; ++j) {
        if (array[j] > array[j + 1]) {
            std::swap(array[j], array[j + 1]);
        }
    }
    step++;
}

// Selection Sort Step
void selectionSortStep(std::vector<int> &array, int step) {
    int minIndex = step;
    for (int j = step + 1; j < array.size(); ++j) {
        if (array[j] < array[minIndex]) {
            minIndex = j;
        }
    }
    if (minIndex != step) {
        std::swap(array[step], array[minIndex]);
    }
}

// Merge Sort Step
void mergeSortStep(std::vector<int> &array, std::stack<MergeSortState> &stack) {
    if (stack.empty()) return;

    MergeSortState &state = stack.top();

    if (state.step == 0) {
        if (state.left >= state.right) {
            stack.pop();
            return;
        }

        state.mid = state.left + (state.right - state.left) / 2;

        stack.push(MergeSortState(state.left, state.mid, 0));
        stack.push(MergeSortState(state.mid + 1, state.right, 0));

        state.step++;
    } else if (state.step == 1) {
        int n1 = state.mid - state.left + 1;
        int n2 = state.right - state.mid;

        state.leftArray.resize(n1);
        state.rightArray.resize(n2);

        for (int i = 0; i < n1; ++i)
            state.leftArray[i] = array[state.left + i];
        for (int i = 0; i < n2; ++i)
            state.rightArray[i] = array[state.mid + 1 + i];

        state.leftIndex = 0;
        state.rightIndex = 0;
        state.mainIndex = state.left;

        state.step++;
    } else if (state.step == 2) {
        if (state.leftIndex < state.leftArray.size() && (state.rightIndex >= state.rightArray.size() || state.leftArray[state.leftIndex] <= state.rightArray[state.rightIndex])) {
            array[state.mainIndex] = state.leftArray[state.leftIndex];
            state.leftIndex++;
        } else if (state.rightIndex < state.rightArray.size()) {
            array[state.mainIndex] = state.rightArray[state.rightIndex];
            state.rightIndex++;
        }
        state.mainIndex++;

        if (state.leftIndex >= state.leftArray.size() && state.rightIndex >= state.rightArray.size()) {
            stack.pop();
        }
    }
}

// Drawing the Bars
void drawBars(sf::RenderWindow &window, const std::vector<int> &array) {
    window.clear();

    for (int i = 0; i < array.size(); ++i) {
        sf::RectangleShape bar(sf::Vector2f(BAR_WIDTH - 1, array[i]));
        bar.setPosition(i * BAR_WIDTH, WINDOW_HEIGHT - array[i]);
        bar.setFillColor(sf::Color::White);
        window.draw(bar);
    }

    window.display();
}

// Menu Function
SortType displayMenu(sf::RenderWindow &window, sf::Font &font) {
    sf::Text title("Sorting Algorithm Visualization", font, 40);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold | sf::Text::Underlined);
    title.setPosition(50, 50);

    sf::Text bubbleSortOption("1. Bubble Sort", font, 30);
    bubbleSortOption.setFillColor(sf::Color::White);
    bubbleSortOption.setPosition(100, 200);

    sf::Text selectionSortOption("2. Selection Sort", font, 30);
    selectionSortOption.setFillColor(sf::Color::White);
    selectionSortOption.setPosition(100, 250);

    sf::Text mergeSortOption("3. Merge Sort", font, 30);
    mergeSortOption.setFillColor(sf::Color::White);
    mergeSortOption.setPosition(100, 300);

    // Background
    sf::RectangleShape background(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(50, 50, 50));

    window.clear();
    window.draw(background);
    window.draw(title);
    window.draw(bubbleSortOption);
    window.draw(selectionSortOption);
    window.draw(mergeSortOption);
    window.display();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return SortType::None;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Num1) {
                    return SortType::Bubble;
                } else if (event.key.code == sf::Keyboard::Num2) {
                    return SortType::Selection;
                } else if (event.key.code == sf::Keyboard::Num3) {
                    return SortType::Merge;
                }
            }
        }
    }

    return SortType::None;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Sorting Visualization");

    sf::Font font;
    if (!font.loadFromFile("OpenSans-Regular.ttf")) { // Load your chosen font file
        std::cerr << "Error loading font\n";
        return -1;
    }

    SortType sortType = displayMenu(window, font);
    if (sortType == SortType::None) {
        return 0;
    }

    std::vector<int> array(NUM_BARS);
    for (int i = 0; i < NUM_BARS; ++i) {
        array[i] = rand() % WINDOW_HEIGHT;
    }

    int currentStep = 0;
    std::stack<MergeSortState> mergeStack;
    if (sortType == SortType::Merge) {
        mergeStack.push(MergeSortState(0, array.size() - 1, 0));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sortType == SortType::Bubble && currentStep < NUM_BARS - 1) {
            bubbleSortStep(array, currentStep);
            drawBars(window, array);
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        } else if (sortType == SortType::Selection && currentStep < NUM_BARS) {
            selectionSortStep(array, currentStep);
            drawBars(window, array);
            currentStep++;
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        } else if (sortType == SortType::Merge && !mergeStack.empty()) {
            mergeSortStep(array, mergeStack);
            drawBars(window, array);
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY));
        }

        if ((sortType == SortType::Bubble && currentStep >= NUM_BARS - 1) ||
            (sortType == SortType::Selection && currentStep >= NUM_BARS) ||
            (sortType == SortType::Merge && mergeStack.empty())) {
            drawBars(window, array);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            window.close();
        }
    }

    return 0;
}
