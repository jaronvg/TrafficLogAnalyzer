# HTTP/HTTPS Traffic Log Analyzer

### Overview 
The desktop application developed was designed to track a given user's web activity given their HTTP/HTTPS traffic logs. These logs contain 
the timestamp, URL (limited), user agent (when applicable), referrer (when applicable), content type, status, and the total size. 
The given data fields were used to develop an algorithm that would eliminate HTTP/HTTPS requests and responses that were deemed "Web Artifacts". 
These artifacts were defined as any request or response not belonging to the website page loaded. All other requests and responses
(images, advertisements, etc) loaded were removed from the resulting trace. As a perfect trace was extremely unlikely given a select amount of data, 
our team aimed for 80% correctness relative to the websites visited by a given user.

### Required Software
1. Electron
2. NodeJS
3. Visual Studio (Recommended)

### Running the Project
To clone and run this repository you'll need Git and Node.js installed on your computer. From the command line in the directory of your choosing:

```bash
# Clone this repository
git clone https://github.com/jaronvg/TrafficLogAnalyzer
# Go into the repository
cd SenProj
# Install dependencies
npm install
# Run the app
npm start
```
