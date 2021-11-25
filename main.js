const {app, BrowserWindow, dialog} = require('electron')
const path = require('path')
process.env['ELECTRON_DISABLE_SECURITY_WARNINGS'] = 'true';
function createWindow () {

  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
        preload: path.join(__dirname, 'preload.js'),
        nodeIntegration: true,
        contextIsolation: false,
    }
  })

  mainWindow.loadFile('index.html')
    mainWindow.on('close', function () {
        const fs = require("fs");
        fs.readdir("traces", (error, dirFiles) => {
            if (error) {
                throw error;
            }
            for (const file of dirFiles) {
                fs.unlink("traces/" + file, error => {
                    if (error) {
                        throw error;
                    }
                });
            }
        });
        try {
            fs.unlinkSync("testSort.txt");
        } catch (error) {
            console.error(error);
        }
    });

}

app.whenReady().then(() => {
  createWindow()

  app.on('activate', function () {

    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', function () {
  if (process.platform !== 'darwin') app.quit()
})


