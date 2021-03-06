# Speed Tracks

We've organized our focus into tracks to address the following concerns:

 * Utilize domain expertise to manage incoming bugs and regression reports
 * Have a clear escalation paths
 * Organize optimization efforts as needed
 * Identify opportunities for improvement and to manage projects

## Tracks Organization

### Memory

This track deals with memory use by Chrome on all platforms. Primary focus is:

 * Gracefully deal with situations where user is out of memory (OOM)
 * Manage memory for idle and backgrounded tabs

#### Links
 
 * [Mailing List](https://groups.google.com/a/chromium.org/forum/#!forum/memory-dev)
 * Performance-Memory [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DMemory)
 * [Docs](https://chromium.googlesource.com/chromium/src/+/master/docs/memory)

### Power

The Power track is concerned with improving power usage for our users.
Collectively, our product has an impact on global greenhouse gas imitions and we
want to do our best to make that efficient. If we can give users a good
experience of not burning laps/hands or making fan noises, we feel like we did a
good job.

#### Links

 * Performance-Power [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DPower)

### Loading

The Loading track focuses on the time between click to the time when you can
interact with a website.

#### Links

 * [Mailing
   List](https://groups.google.com/a/chromium.org/forum/#!forum/loading-dev)
 * Performance-Loading [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DLoading)

### Responsiveness

Responsiveness track focuses on making sure all websites have smooth transitions
by serving 60fps, and that the click to action time is not noticible.

#### Links

 * Performance-Responsiveness [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DResponsiveness)

### Binary Size

Chrome has an update for you at least every six weeks. Since we do that for all
of our users, we want to be nice to our users where downloading updates costs
real money. We also don't want to hog all of the disk space on low end phones.
So we focus attention on making sure we don't include bits in our update that
are not necessary for users.

#### Links

 * [Mailing List](://groups.google.com/a/chromium.org/forum/#!forum/binary-size)
 * Performance-Size [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DSize)

### Data Usage

Data Usage is a focus on the question: Do the user see or need every byte
downloaded? By looking at this, we can save user's cost of data, time to load,
memory and power. 

#### Links

 * Performance-Data [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DData)

### Startup, Omnibox, Browser UI, etc.

There are a handful of performance angles that don't fit into the tracks already
mentioned. Historically, we've put these into a "Browser" bucket as that's
descriptive of what's left over. These are things like making sure the Omnibox
experience on Chrome is fast, making sure all of the Chrome UI, e.g. Settings,
is fast and that the browser startup and session restore doesn't allow users to
make coffee before they use the browser.

#### Links

 * Performance-Browser [Bug
   Queue](https://bugs.chromium.org/p/chromium/issues/list?can=2&q=Performance%3DBrowser)
