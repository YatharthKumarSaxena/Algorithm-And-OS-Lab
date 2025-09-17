
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <string>
#include <cmath>
#include <climits>

using namespace std;

const char* FONT_FILE = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"; 
const int WINDOW_W = 1100;
const int WINDOW_H = 700;
const int MAX_PROCESSES = 40; 

int stoi_safe(const string &s) {
    try {
        return std::stoi(s);
    } catch (...) { return 0; }
}

template<typename T>
class DynamicArray {
    T* data;
    int sz;
    int cap;
public:
    DynamicArray(): data(nullptr), sz(0), cap(0) {}
    ~DynamicArray(){ if(data) delete[] data; }
    void reserve(int newcap) {
        if(newcap <= cap) return;
        T* nd = new T[newcap];
        for(int i=0;i<sz;i++) nd[i] = data[i];
        if(data) delete[] data;
        data = nd;
        cap = newcap;
    }
    void push_back(const T &val) {
        if(sz == cap) reserve(cap ? cap*2 : 8);
        data[sz++] = val;
    }
    T& operator[](int idx) { return data[idx]; }
    int size() const { return sz; }
    void clear() { sz = 0; }
};
 
class IntQueue {
    int *arr;
    int head, tail, cnt, capacity;
public:
    IntQueue(int cap=64) {
        capacity = cap;
        arr = new int[capacity];
        head = 0; tail = 0; cnt = 0;
    }
    ~IntQueue(){ delete[] arr; }
    bool empty() const { return cnt == 0; }
    void push(int v) {
        if(cnt == capacity) {
            // resize
            int newcap = capacity * 2;
            int *narr = new int[newcap];
            for(int i=0;i<cnt;i++) narr[i] = arr[(head + i) % capacity];
            delete[] arr;
            arr = narr;
            capacity = newcap;
            head = 0; tail = cnt;
        }
        arr[tail] = v;
        tail = (tail + 1) % capacity;
        cnt++;
    }
    int pop() {
        if(cnt==0) return -1;
        int v = arr[head];
        head = (head + 1) % capacity;
        cnt--;
        return v;
    }
    int size() const { return cnt; }
};

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int start;   // first start time
    int finish;  // finish time
    int waiting;
    int turnaround;
};

struct GanttBlock {
    int pid;
    int start;
    int end;
};

// Simple text input box
class InputBox {
    sf::RectangleShape box;
    sf::Text text;
    string content;
    bool active;
public:
    InputBox() { active = false; }
    void init(float x, float y, float w, float h, sf::Font &font) {
        box.setPosition(x,y);
        box.setSize({w,h});
        box.setFillColor(sf::Color(245,245,245));
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color::Black);
        text.setFont(font);
        text.setCharacterSize((unsigned int)(h*0.5));
        text.setFillColor(sf::Color::Black);
        text.setPosition(x+6, y+ (h/6.0f));
        content = "";
    }
    void handleEvent(const sf::Event &ev) {
        if(ev.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f m(ev.mouseButton.x, ev.mouseButton.y);
            active = box.getGlobalBounds().contains(m);
            if(!active) { /* do nothing */ }
        }
        if(active && ev.type == sf::Event::TextEntered) {
            uint32_t c = ev.text.unicode;
            if(c == 8) { // backspace
                if(!content.empty()) content.pop_back();
            } else if(c >= 32 && c < 127) {
                content.push_back((char)c);
            }
            text.setString(content);
        }
    }
    void draw(sf::RenderWindow &w) {
        w.draw(box);
        w.draw(text);
    }
    void setString(const string &s) { content = s; text.setString(content); }
    string getString() const { return content; }
    void clear() { content.clear(); text.setString(content); }
    void setActive(bool a) { active = a; }
};

// Simple button
class Button {
    sf::RectangleShape box;
    sf::Text label;
public:
    void init(float x, float y, float w, float h, sf::Font &font, const string &s) {
        box.setPosition(x,y); box.setSize({w,h});
        box.setFillColor(sf::Color(200,200,200));
        box.setOutlineColor(sf::Color::Black);
        box.setOutlineThickness(2);
        label.setFont(font); label.setString(s);
        label.setCharacterSize((unsigned int)(h*0.45));
        label.setFillColor(sf::Color::Black);
        // center label
        sf::FloatRect lb = label.getLocalBounds();
        label.setPosition(x + (w - lb.width)/2 - lb.left, y + (h - lb.height)/2 - lb.top);
    }
    void draw(sf::RenderWindow &w) { w.draw(box); w.draw(label); }
    bool isClicked(const sf::Event &ev) {
        if(ev.type != sf::Event::MouseButtonPressed) return false;
        sf::Vector2f m(ev.mouseButton.x, ev.mouseButton.y);
        return box.getGlobalBounds().contains(m);
    }
};

// Utility: print results to console
void printResults(Process procs[], int n) {
    double sumWT = 0.0, sumTAT = 0.0;
    cout << "PID\tArrival\tBurst\tWT\tTAT\n";
    for(int i=0;i<n;i++) {
        cout << "P" << procs[i].id << "\t" << procs[i].arrival << "\t" << procs[i].burst
             << "\t" << procs[i].waiting << "\t" << procs[i].turnaround << "\n";
        sumWT += procs[i].waiting;
        sumTAT += procs[i].turnaround;
    }
    cout << "Avg WT = " << (sumWT / n) << "\n";
    cout << "Avg TAT = " << (sumTAT / n) << "\n";
}

// RR (using simple time-slice simulation), returns gantt chart and last time
int schedule_RR(Process original[], int n, int quantum, DynamicArray<GanttBlock> &gantt_out, Process outProcs[]) {
    // copy processes into local array
    Process p[MAX_PROCESSES];
    for(int i=0;i<n;i++) {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].start = -1;
        p[i].finish = -1;
    }

    IntQueue q(16);
    int time = 0;
    int completed = 0;
    bool added[MAX_PROCESSES] = {false};

    // initially add processes that have arrival==0
    for(int i=0;i<n;i++) if(p[i].arrival <= time && !added[i]) { q.push(i); added[i]=true; }

    while(completed < n) {
        // if queue empty, advance time to next arrival
        if(q.empty()) {
            int nextArr = INT_MAX;
            for(int i=0;i<n;i++) if(!added[i]) nextArr = std::min(nextArr, p[i].arrival);
            if(nextArr==INT_MAX) break;
            time = nextArr;
            for(int i=0;i<n;i++) if(p[i].arrival <= time && !added[i]) { q.push(i); added[i]=true; }
            continue;
        }

        int idx = q.pop();
        if(p[idx].start == -1) p[idx].start = time;
        int exec = std::min(quantum, p[idx].remaining);
        // push Gantt block
        GanttBlock gb; gb.pid = p[idx].id; gb.start = time; gb.end = time + exec;
        gantt_out.push_back(gb);

        time += exec;
        p[idx].remaining -= exec;
        // add newly arrived processes during this timeslice
        for(int i=0;i<n;i++) if(!added[i] && p[i].arrival <= time) { q.push(i); added[i]=true; }
        if(p[idx].remaining > 0) {
            q.push(idx);
        } else {
            p[idx].finish = time;
            p[idx].turnaround = p[idx].finish - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;
        }
    }

    // move results to outProcs
    for(int i=0;i<n;i++) outProcs[i] = p[i];
    return time;
}

// SJF Non-preemptive
int schedule_SJF_NP(Process original[], int n, DynamicArray<GanttBlock> &gantt_out, Process outProcs[]) {
    Process p[MAX_PROCESSES];
    for(int i=0;i<n;i++) {
        p[i] = original[i];
        p[i].remaining = p[i].burst;
        p[i].start = -1;
        p[i].finish = -1;
    }
    int time = 0;
    int done = 0;
    bool used[MAX_PROCESSES] = {false};

    while(done < n) {
        // find processes arrived, not used
        int idx = -1;
        int minBurst = INT_MAX;
        for(int i=0;i<n;i++) {
            if(!used[i] && p[i].arrival <= time) {
                if(p[i].burst < minBurst) { minBurst = p[i].burst; idx = i; }
                else if(p[i].burst == minBurst) {
                    if(p[i].arrival < p[idx].arrival) idx = i;
                }
            }
        }
        if(idx == -1) {
            // advance to next arrival
            int nxt = INT_MAX;
            for(int i=0;i<n;i++) if(!used[i]) nxt = std::min(nxt, p[i].arrival);
            time = nxt;
            continue;
        }
        p[idx].start = time;
        GanttBlock gb; gb.pid = p[idx].id; gb.start = time; gb.end = time + p[idx].burst;
        gantt_out.push_back(gb);
        time += p[idx].burst;
        p[idx].finish = time;
        p[idx].turnaround = p[idx].finish - p[idx].arrival;
        p[idx].waiting = p[idx].turnaround - p[idx].burst;
        used[idx] = true;
        done++;
    }

    for(int i=0;i<n;i++) outProcs[i] = p[i];
    return time;
}

// SJF Preemptive (SRTF)
int schedule_SJF_P(Process original[], int n, DynamicArray<GanttBlock> &gantt_out, Process outProcs[]) {
    Process p[MAX_PROCESSES];
    for(int i=0;i<n;i++) { p[i] = original[i]; p[i].remaining = p[i].burst; p[i].start = -1; p[i].finish = -1; }

    int time = 0;
    int completed = 0;
    int last_pid = -1;
    // simulate each time unit
    while(completed < n) {
        // find index with min remaining among arrived
        int idx = -1; int minRem = INT_MAX;
        for(int i=0;i<n;i++) {
            if(p[i].arrival <= time && p[i].remaining > 0) {
                if(p[i].remaining < minRem) { minRem = p[i].remaining; idx = i; }
                else if(p[i].remaining == minRem) {
                    if(p[i].arrival < p[idx].arrival) idx = i;
                }
            }
        }
        if(idx == -1) {
            time++;
            continue;
        }
        // if switching process, create new block
        if(last_pid == -1 || last_pid != p[idx].id) {
            // start new block
            GanttBlock gb; gb.pid = p[idx].id; gb.start = time; gb.end = time+1;
            gantt_out.push_back(gb);
        } else {
            // extend last block's end
            GanttBlock &last = gantt_out[gantt_out.size()-1];
            last.end += 1;
        }
        if(p[idx].start == -1) p[idx].start = time;
        p[idx].remaining -= 1;
        time += 1;
        if(p[idx].remaining == 0) {
            p[idx].finish = time;
            p[idx].turnaround = p[idx].finish - p[idx].arrival;
            p[idx].waiting = p[idx].turnaround - p[idx].burst;
            completed++;
        }
        last_pid = p[idx].id;
    }
    for(int i=0;i<n;i++) outProcs[i] = p[i];
    return time;
}

void animateGantt(sf::RenderWindow &window, sf::Font &font, DynamicArray<GanttBlock> &gchart, int totalTime) {
    // Prepare colors
    sf::Color colors[12] = {
        sf::Color(200, 80, 80), sf::Color(80, 200, 80), sf::Color(80,80,200),
        sf::Color(200,200,80), sf::Color(200,80,200), sf::Color(80,200,200),
        sf::Color(160,100,40), sf::Color(100,160,40), sf::Color(40,100,160),
        sf::Color(150,50,130), sf::Color(50,130,150), sf::Color(130,150,50)
    };

    // layout
    const int marginLeft = 60;
    const int marginTop = 100;
    const int chartH = 120;
    int width = window.getSize().x - marginLeft - 40;
    if(totalTime <= 0) totalTime = 1;
    float unitW = (float)width / totalTime;

    // Show legend of processes used
    // find unique processes
    int usedP[MAX_PROCESSES]; int usedCnt = 0;
    for(int i=0;i<gchart.size();i++){
        int pid = gchart[i].pid;
        bool found = false;
        for(int j=0;j<usedCnt;j++) if(usedP[j]==pid) { found=true; break; }
        if(!found) usedP[usedCnt++] = pid;
    }

    // Animation variables
    int currentBlock = 0;
    float revealProgress = 0.0f; // 0..1 within block
    sf::Clock clock;
    float speed = 1.0f; // time multiplier, 1 second -> 1 time unit. adjust if needed
    bool paused = false;

    while(window.isOpen()) {
        sf::Event ev;
        while(window.pollEvent(ev)) {
            if(ev.type == sf::Event::Closed) return;
            if(ev.type == sf::Event::KeyPressed) {
                if(ev.key.code == sf::Keyboard::Space) paused = !paused;
                if(ev.key.code == sf::Keyboard::Right) { speed *= 1.5f; }
                if(ev.key.code == sf::Keyboard::Left) { speed = std::max(0.1f, speed/1.5f); }
                if(ev.key.code == sf::Keyboard::Escape) return;
            }
        }

        if(!paused) {
            float dt = clock.restart().asSeconds();
            revealProgress += dt * speed;
        } else {
            clock.restart();
        }

        // Convert real elapsed into simulated timeline
        static float accumulated = 0.0f;

        static float simulatedTime = 0.0f;
        if(!paused) {
            simulatedTime += 0.02f * speed; // increment by .02 time-units per frame times speed
            if(simulatedTime > totalTime) simulatedTime = (float)totalTime;
        }

        window.clear(sf::Color(250,250,250));

        // Title
        sf::Text title("Gantt Chart (Space: pause/resume, Left/Right: slow/fast, Esc: exit)", font, 18);
        title.setPosition(20, 10);
        title.setFillColor(sf::Color::Black);
        window.draw(title);

        // Draw axes
        sf::RectangleShape axisX(sf::Vector2f(width+2, 2));
        axisX.setPosition(marginLeft, marginTop + chartH + 10);
        axisX.setFillColor(sf::Color::Black);
        window.draw(axisX);

        // Draw time ticks
        for(int t=0;t<=totalTime;t++) {
            float x = marginLeft + t * unitW;
            sf::RectangleShape tick(sf::Vector2f(1,8));
            tick.setPosition(x, marginTop + chartH + 10);
            tick.setFillColor(sf::Color::Black);
            window.draw(tick);
            // label every few ticks
            if(totalTime <= 40 || t % (std::max(1,totalTime/40)) == 0) {
                sf::Text tlabel(to_string(t), font, 12);
                tlabel.setPosition(x - 6, marginTop + chartH + 18);
                tlabel.setFillColor(sf::Color::Black);
                window.draw(tlabel);
            }
        }

        // Draw gantt blocks according to simulatedTime progress
        float showUpTo = simulatedTime; 
        for(int i=0;i<gchart.size();i++) {
            float s = (float)gchart[i].start;
            float e = (float)gchart[i].end;
            float drawStart = s;
            float drawEnd = std::min(e, showUpTo);
            if(drawEnd <= drawStart) continue;
            float x = marginLeft + drawStart * unitW;
            float w = (drawEnd - drawStart) * unitW;
            sf::RectangleShape rect(sf::Vector2f(w - 1.0f, chartH - 20));
            int colorIndex = (gchart[i].pid - 1) % 12;
            rect.setFillColor(colors[colorIndex]);
            rect.setPosition(x, marginTop);
            rect.setOutlineThickness(1);
            rect.setOutlineColor(sf::Color::Black);
            window.draw(rect);
            // label
            sf::Text lab("P" + to_string(gchart[i].pid), font, 16);
            lab.setPosition(x + 6, marginTop + (chartH - 20)/2 - 12);
            lab.setFillColor(sf::Color::Black);
            window.draw(lab);
        }

        // Legend
        for(int i=0;i<usedCnt;i++) {
            int pid = usedP[i];
            sf::RectangleShape legendRect(sf::Vector2f(24,16));
            legendRect.setPosition(marginLeft + i*110, marginTop + chartH + 60);
            legendRect.setFillColor(colors[(pid-1)%12]);
            legendRect.setOutlineThickness(1); legendRect.setOutlineColor(sf::Color::Black);
            window.draw(legendRect);
            sf::Text ltext("P" + to_string(pid), font, 14);
            ltext.setPosition(marginLeft + i*110 + 30, marginTop + chartH + 56);
            ltext.setFillColor(sf::Color::Black);
            window.draw(ltext);
        }

        // Controls info
        sf::Text info("Simulated time: " + to_string((int)simulatedTime) + " / " + to_string(totalTime) , font, 14);
        info.setPosition(marginLeft, marginTop + chartH + 30);
        info.setFillColor(sf::Color::Black);
        window.draw(info);

        window.display();

        if(simulatedTime >= totalTime) {
            // keep window until user closes or presses Esc
            sf::sleep(sf::milliseconds(100));
            // still loop to accept events; no further animation
        }
    }
}

// ----------------------------- Main Application -----------------------------
int main() {
    // Load font
    sf::Font font;
    if(!font.loadFromFile(FONT_FILE)) {
        cerr << "Failed to load font at " << FONT_FILE << ". Update FONT_FILE path inside source.\n";
        return 1;
    }

    // Prepare main window
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "CPU Scheduling - RR & SJF (SFML) ");
    window.setFramerateLimit(60);

    // Input controls
    InputBox numBox;
    numBox.init(220, 30, 80, 36, font);
    numBox.setString("3"); // default

    Button btnSetCount; btnSetCount.init(310, 30, 120, 36, font, "Set Count");
    Button btnRun; btnRun.init(20, 560, 120, 40, font, "Run");
    Button btnClear; btnClear.init(150, 560, 120, 40, font, "Clear");

    // Algorithm radio-like buttons (simple)
    Button bRR; bRR.init(20, 80, 160, 36, font, "Round Robin (RR)");
    Button bSJFNP; bSJFNP.init(200, 80, 220, 36, font, "SJF Non-Preemptive");
    Button bSJFP; bSJFP.init(440, 80, 220, 36, font, "SJF Preemptive (SRTF)");
    int algo = 0; // 0 RR, 1 SJFNP, 2 SJFP

    // quantum box for RR
    InputBox quantumBox; quantumBox.init(700, 80, 80, 36, font);
    quantumBox.setString("2");

    // process input boxes dynamic
    int procCount = 3;
    int displayed = 0;
    InputBox atBoxes[MAX_PROCESSES];
    InputBox btBoxes[MAX_PROCESSES];
    for(int i=0;i<MAX_PROCESSES;i++){
        int baseY = 140 + i*36;
        atBoxes[i].init(180, baseY, 100, 30, font);
        btBoxes[i].init(320, baseY, 100, 30, font);
        atBoxes[i].setString("0");
        btBoxes[i].setString("1");
    }
    displayed = procCount;

    // When user clicks set count: we'll change displayed but inputs exist
    bool runningScheduling = false;

    while(window.isOpen()) {
        sf::Event ev;
        while(window.pollEvent(ev)) {
            if(ev.type == sf::Event::Closed) { window.close(); return 0; }
            numBox.handleEvent(ev);
            quantumBox.handleEvent(ev);
            for(int i=0;i<displayed;i++){ atBoxes[i].handleEvent(ev); btBoxes[i].handleEvent(ev); }
            if(bRR.isClicked(ev)) algo = 0;
            if(bSJFNP.isClicked(ev)) algo = 1;
            if(bSJFP.isClicked(ev)) algo = 2;
            if(btnSetCount.isClicked(ev)) {
                string s = numBox.getString();
                int v = stoi_safe(s);
                if(v < 1) v = 1;
                if(v > MAX_PROCESSES) v = MAX_PROCESSES;
                procCount = v; displayed = procCount;
            }
            if(btnClear.isClicked(ev)) {
                for(int i=0;i<displayed;i++){ atBoxes[i].setString("0"); btBoxes[i].setString("1"); }
                numBox.setString("0"); quantumBox.setString("2");
            }

            if(btnRun.isClicked(ev)) {
                // Read inputs into processes
                if(procCount <= 0) { cerr << "Set number of processes > 0\n"; continue; }
                Process procs[MAX_PROCESSES];
                bool bad = false;
                for(int i=0;i<procCount;i++){
                    string sa = atBoxes[i].getString();
                    string sb = btBoxes[i].getString();
                    if(sa.empty()) sa = "0";
                    if(sb.empty()) sb = "0";
                    int a = stoi_safe(sa);
                    int b = stoi_safe(sb);
                    if(b < 0) b = 0;
                    if(a < 0) a = 0;
                    procs[i].id = i+1;
                    procs[i].arrival = a;
                    procs[i].burst = b;
                    procs[i].remaining = b;
                    procs[i].start = -1;
                    procs[i].finish = -1;
                    procs[i].waiting = 0;
                    procs[i].turnaround = 0;
                }
                // If all bursts zero, skip
                bool allZero = true;
                for(int i=0;i<procCount;i++) if(procs[i].burst > 0) { allZero = false; break; }
                if(allZero) { cerr << "All bursts are zero - nothing to schedule\n"; continue; }

                // Run selected scheduler
                DynamicArray<GanttBlock> gantt;
                gantt.clear();
                Process result[MAX_PROCESSES];
                int totalTime = 0;
                if(algo == 0) { // RR
                    int qv = stoi_safe(quantumBox.getString());
                    if(qv <= 0) qv = 1;
                    totalTime = schedule_RR(procs, procCount, qv, gantt, result);
                } else if(algo == 1) {
                    totalTime = schedule_SJF_NP(procs, procCount, gantt, result);
                } else {
                    totalTime = schedule_SJF_P(procs, procCount, gantt, result);
                }

                // Print results
                cout << "Results for ";
                if(algo==0) cout << "Round Robin (q=" << quantumBox.getString() << ")\n";
                else if(algo==1) cout << "SJF Non-Preemptive\n";
                else cout << "SJF Preemptive (SRTF)\n";
                printResults(result, procCount);
                cout << "Gantt blocks: " << gantt.size() << ", totalTime: " << totalTime << "\n";

                // open new window for animation (modal)
                sf::RenderWindow anim(sf::VideoMode(1000,700), "Gantt Animation (Press Space to pause/resume)");
                anim.setFramerateLimit(60);
                animateGantt(anim, font, gantt, totalTime);
                // after animateGantt returns (user closed anim window), continue main loop
            }
        }

        // Draw UI
        window.clear(sf::Color(245,245,245));
        // titles
        sf::Text title("CPU Scheduling Simulator (RR, SJF NP, SJF P)", font, 20);
        title.setPosition(20, 2); title.setFillColor(sf::Color::Black);
        window.draw(title);

        // labels
        sf::Text t1("Processes count:", font, 16); t1.setPosition(20, 30); window.draw(t1);
        numBox.draw(window);
        btnSetCount.draw(window);

        // algorithm selection
        bRR.draw(window); bSJFNP.draw(window); bSJFP.draw(window);
        // highlight selected
        sf::RectangleShape selRect; selRect.setSize({4,36}); selRect.setFillColor(sf::Color::Blue);
        if(algo==0) { selRect.setPosition(18,80); window.draw(selRect); }
        if(algo==1) { selRect.setPosition(198,80); window.draw(selRect); }
        if(algo==2) { selRect.setPosition(438,80); window.draw(selRect); }

        // quantum
        sf::Text qlab("Quantum (RR):", font, 16); qlab.setPosition(610, 80); window.draw(qlab);
        quantumBox.draw(window);

        // process table header
        sf::Text header("Idx   Arrival    Burst", font, 18); header.setPosition(20, 110); window.draw(header);
        // input rows
        for(int i=0;i<displayed;i++) {
            sf::Text idxTxt("P" + to_string(i+1), font, 16); idxTxt.setPosition(20, 140 + i*36); idxTxt.setFillColor(sf::Color::Black);
            window.draw(idxTxt);
            atBoxes[i].draw(window);
            btBoxes[i].draw(window);
        }

        // buttons
        btnRun.draw(window);
        btnClear.draw(window);

        // footnote
        sf::Text foot("Note: Use correct font path inside source if labels don't appear. After Run, animation opens in new window.", font, 12);
        foot.setPosition(20, 620); foot.setFillColor(sf::Color::Black);
        window.draw(foot);

        window.display();
    }

    return 0;
}
