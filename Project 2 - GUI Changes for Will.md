**GUI Changes for Will**

Hey Will, here's a quick rundown of what changed in the GUI plan since the wireframe we submitted.

Updated wireframe: https://claude.ai/artifact/3KMq9wSHczg3yvrS1x3kiQ

Updated wireframe image:

Full GUI details: `Project 2 - GUI Summary.md` in the repo.

**1. No more Safe/Unsafe**

Every run plants 100 test questions into the training data so we can check that the structures actually find them, which means "Safe" could never show. The results screen now says: "x benchmark questions were found in the training set (includes the 100 planted test questions)". It counts questions, not occurrences.

**2. Two new lines on the results screen**

The results screen gets "Planted found: x/100" and "Both structures agree: yes/no". The two Time/Space/lookup boxes stay the same.

**3. New loading screen**

There's a new "Loading data…" screen between Start and the bars. It shows while the CSVs get read and cleaned.

**4. Progress bars move in 5 steps**

We time everything 5 times and report the median, so each bar fills one fifth per run. You don't need a second thread. Measurement gives you three functions: time one build, time one lookup run, and median. You call one run, redraw, then call the next.

Order: HT builds, HT lookups, SA builds, SA lookups. Each Space bar shows up once that structure's builds finish.

**5. No planting toggle and no column picker**

Every GUI run plants 100, and the text columns are fixed (the README will say which ones). The command-line version keeps a `--plant` option for the small sample files.

**6. Your call: how the user picks files**

SFML has no file dialog built in. You have two options:

* **Option A:** tinyfiledialogs (one .c + one .h file, zlib license). It opens the normal Open window on Windows, Mac, and Linux.
* **Option B:** a plain text box where you type the path.

Let me know which one you want.

**A few other things**

* The GUI has to run on a real computer, since codespaces have no screen.
* Timing numbers should come from the optimized build, not the sanitizer one.
* If a single run takes more than a few seconds, Windows might show "Not responding". We'll know once we see how big the corpus is.
* There's also a command-line program (`app/main.cpp`) that runs the same sequence and prints the results, so we can test without the GUI.

We'll list all of this in the report under changes after the proposal.
