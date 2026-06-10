#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QFont>
#include <map>
#include <vector>
#include <string>
#include <sqlite3.h>

// Database connection
sqlite3* db = nullptr;
const char* dbPath = "attendance.db";

// Initialize database and create table
bool initDatabase() {
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        return false;
    }
    
    const char* createTableSQL = "CREATE TABLE IF NOT EXISTS attendance("
                                  "id TEXT PRIMARY KEY, "
                                  "name TEXT NOT NULL, "
                                  "status TEXT NOT NULL, "
                                  "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";
    
    char* errMsg = nullptr;
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

// Insert attendance record
bool insertAttendance(const std::string& id, const std::string& name, const std::string& status) {
    std::string insertSQL = "INSERT OR REPLACE INTO attendance (id, name, status) VALUES ('" +
                            id + "', '" + name + "', '" + status + "');";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

// Search attendance record
std::string searchAttendance(const std::string& id) {
    std::string query = "SELECT name, status FROM attendance WHERE id = '" + id + "';";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(stmt, 0);
        const char* status = (const char*)sqlite3_column_text(stmt, 1);
        sqlite3_finalize(stmt);
        return std::string(name) + " | " + std::string(status);
    }
    
    sqlite3_finalize(stmt);
    return "";
}

// Get all attendance records
std::vector<std::pair<std::string, std::pair<std::string, std::string>>> getAllRecords() {
    std::vector<std::pair<std::string, std::pair<std::string, std::string>>> records;
    
    const char* query = "SELECT id, name, status FROM attendance ORDER BY timestamp DESC;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = (const char*)sqlite3_column_text(stmt, 0);
            std::string name = (const char*)sqlite3_column_text(stmt, 1);
            std::string status = (const char*)sqlite3_column_text(stmt, 2);
            records.push_back({id, {name, status}});
        }
    }
    
    sqlite3_finalize(stmt);
    return records;
}

// Close database connection
void closeDatabase() {
    if (db) {
        sqlite3_close(db);
    }
}

// Main Attendance System Window Class
class AttendanceWindow : public QMainWindow {
    Q_OBJECT

public:
    AttendanceWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Attendance Management System");
        setGeometry(100, 100, 700, 650);
        
        // Central widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Title
        QLabel *titleLabel = new QLabel("Attendance Management System");
        QFont titleFont("Arial", 16, QFont::Bold);
        titleLabel->setFont(titleFont);
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        // Separator
        QFrame *line1 = new QFrame();
        line1->setFrameShape(QFrame::HLine);
        mainLayout->addWidget(line1);
        
        // Mark Attendance Group
        QGroupBox *markGroup = new QGroupBox("Mark Attendance");
        QVBoxLayout *markLayout = new QVBoxLayout();
        
        // Student ID input
        QHBoxLayout *idLayout = new QHBoxLayout();
        idLayout->addWidget(new QLabel("Student ID:"));
        inputStudentID = new QLineEdit();
        inputStudentID->setMaximumWidth(200);
        idLayout->addWidget(inputStudentID);
        idLayout->addStretch();
        markLayout->addLayout(idLayout);
        
        // Student Name input
        QHBoxLayout *nameLayout = new QHBoxLayout();
        nameLayout->addWidget(new QLabel("Student Name:"));
        inputStudentName = new QLineEdit();
        inputStudentName->setMaximumWidth(200);
        nameLayout->addWidget(inputStudentName);
        nameLayout->addStretch();
        markLayout->addLayout(nameLayout);
        
        // Attendance Status dropdown
        QHBoxLayout *statusLayout = new QHBoxLayout();
        statusLayout->addWidget(new QLabel("Status:"));
        statusCombo = new QComboBox();
        statusCombo->addItem("Present");
        statusCombo->addItem("Absent");
        statusCombo->setMaximumWidth(200);
        statusLayout->addWidget(statusCombo);
        statusLayout->addStretch();
        markLayout->addLayout(statusLayout);
        
        // Buttons for marking attendance
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        QPushButton *btnMark = new QPushButton("Mark Attendance");
        QPushButton *btnClear = new QPushButton("Clear");
        
        buttonLayout->addWidget(btnMark);
        buttonLayout->addWidget(btnClear);
        buttonLayout->addStretch();
        markLayout->addLayout(buttonLayout);
        
        markGroup->setLayout(markLayout);
        mainLayout->addWidget(markGroup);
        
        // Search Group
        QGroupBox *searchGroup = new QGroupBox("Search Attendance");
        QHBoxLayout *searchLayout = new QHBoxLayout();
        
        searchLayout->addWidget(new QLabel("Search by ID:"));
        inputSearchID = new QLineEdit();
        inputSearchID->setMaximumWidth(200);
        searchLayout->addWidget(inputSearchID);
        
        QPushButton *btnSearch = new QPushButton("Search");
        searchLayout->addWidget(btnSearch);
        searchLayout->addStretch();
        
        searchGroup->setLayout(searchLayout);
        mainLayout->addWidget(searchGroup);
        
        // Separator
        QFrame *line2 = new QFrame();
        line2->setFrameShape(QFrame::HLine);
        mainLayout->addWidget(line2);
        
        // Output section
        mainLayout->addWidget(new QLabel("Attendance Records:"));
        outputText = new QTextEdit();
        outputText->setReadOnly(true);
        mainLayout->addWidget(outputText);
        
        // View All button
        QPushButton *btnViewAll = new QPushButton("View All Records");
        mainLayout->addWidget(btnViewAll);
        
        // Connect signals
        connect(btnMark, &QPushButton::clicked, this, &AttendanceWindow::onMarkAttendance);
        connect(btnClear, &QPushButton::clicked, this, &AttendanceWindow::onClear);
        connect(btnSearch, &QPushButton::clicked, this, &AttendanceWindow::onSearch);
        connect(btnViewAll, &QPushButton::clicked, this, &AttendanceWindow::onViewAll);
    }

private slots:
    void onMarkAttendance() {
        QString studentID = inputStudentID->text();
        QString studentName = inputStudentName->text();
        QString status = statusCombo->currentText();
        
        if (studentID.isEmpty() || studentName.isEmpty()) {
            outputText->append("Error: Please enter Student ID and Name.");
            return;
        }
        
        // Store attendance record in database
        std::string idStr = studentID.toStdString();
        std::string nameStr = studentName.toStdString();
        std::string statusStr = status.toStdString();
        
        if (insertAttendance(idStr, nameStr, statusStr)) {
            outputText->append(QString("✓ Attendance marked for %1 (%2) - %3")
                              .arg(studentName, studentID, status));
            inputStudentID->clear();
            inputStudentName->clear();
            statusCombo->setCurrentIndex(0);
        } else {
            outputText->append("Error: Failed to save attendance record.");
        }
    }
    
    void onClear() {
        inputStudentID->clear();
        inputStudentName->clear();
        statusCombo->setCurrentIndex(0);
    }
    
    void onSearch() {
        QString searchID = inputSearchID->text();
        if (searchID.isEmpty()) {
            outputText->append("Error: Please enter a Student ID to search.");
            return;
        }
        
        std::string idStr = searchID.toStdString();
        std::string result = searchAttendance(idStr);
        
        if (!result.empty()) {
            outputText->append(QString("\n--- Search Result ---"));
            outputText->append(QString("Student ID: %1").arg(searchID));
            
            size_t pos = result.find(" | ");
            std::string name = result.substr(0, pos);
            std::string status = result.substr(pos + 3);
            
            outputText->append(QString("Name: %1").arg(QString::fromStdString(name)));
            outputText->append(QString("Status: %1").arg(QString::fromStdString(status)));
            outputText->append(QString("-------------------\n"));
        } else {
            outputText->append(QString("No record found for Student ID: %1").arg(searchID));
        }
    }
    
    void onViewAll() {
        auto records = getAllRecords();
        
        if (records.empty()) {
            outputText->setText("No attendance records yet.");
            return;
        }
        
        outputText->clear();
        outputText->append("=========== ALL ATTENDANCE RECORDS ===========");
        outputText->append("");
        
        int count = 1;
        for (const auto& record : records) {
            outputText->append(QString("%1. ID: %2 | Name: %3 | Status: %4")
                              .arg(count)
                              .arg(QString::fromStdString(record.first))
                              .arg(QString::fromStdString(record.second.first))
                              .arg(QString::fromStdString(record.second.second)));
            count++;
        }
        outputText->append("");
        outputText->append(QString("=============================================="));
        outputText->append(QString("Total Records: %1").arg(records.size()));
    }

private:
    QLineEdit *inputStudentID;
    QLineEdit *inputStudentName;
    QComboBox *statusCombo;
    QLineEdit *inputSearchID;
    QTextEdit *outputText;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Initialize database
    if (!initDatabase()) {
        return -1;
    }
    
    AttendanceWindow window;
    window.show();
    
    int result = app.exec();
    
    // Close database connection
    closeDatabase();
    
    return result;
}

#include "ss.moc"