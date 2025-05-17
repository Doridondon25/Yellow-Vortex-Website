import { initializeApp } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-app.js";
import { getDatabase, ref, set } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-database.js";

// Initialize Firebase
const firebaseConfig = {
  apiKey: "AIzaSyC59vWDbdtLqfRTlqQWpdrrt2Eia36PqH8",
  authDomain: "yellow-vortex.firebaseapp.com",
  databaseURL: "https://yellow-vortex-default-rtdb.europe-west1.firebasedatabase.app",
  projectId: "yellow-vortex",
  storageBucket: "yellow-vortex.firebasestorage.app",
  messagingSenderId: "1007464214882",
  appId: "1:1007464214882:web:c1e7b0e1a266309b94bc3b",
  measurementId: "G-V76FRQK0FR"
};
const app = initializeApp(firebaseConfig);
const db = getDatabase(app); // Initialize the database instance

// Appointments data
const appointments = {
  appointment1: { date: "2025-05-01", time: "09:00", doctor: "Dr. Smith", location: "Tel Aviv", occupied: false },
  appointment2: { date: "2025-05-01", time: "10:00", doctor: "Dr. Jones", location: "Haifa", occupied: true },
  appointment3: { date: "2025-05-01", time: "11:00", doctor: "Dr. Lee", location: "Hod Hasharon", occupied: false },
  appointment4: { date: "2025-05-02", time: "09:00", doctor: "Dr. Kim", location: "Jerusalem", occupied: false },
  appointment5: { date: "2025-05-02", time: "10:30", doctor: "Dr. Green", location: "Be'er Sheva", occupied: true },
  appointment6: { date: "2025-05-03", time: "08:00", doctor: "Dr. Amir", location: "Netanya", occupied: false },
  appointment7: { date: "2025-05-03", time: "09:30", doctor: "Dr. Levi", location: "Ashdod", occupied: true },
  appointment8: { date: "2025-05-04", time: "13:00", doctor: "Dr. Tal", location: "Eilat", occupied: false },
  appointment9: { date: "2025-05-05", time: "14:00", doctor: "Dr. Cohen", location: "Tiberias", occupied: true },
  appointment10: { date: "2025-05-06", time: "15:00", doctor: "Dr. Dana", location: "Nazareth", occupied: false },
  appointment11: { date: "2025-05-06", time: "10:00", doctor: "Dr. Peretz", location: "Ramat Gan", occupied: true },
  appointment12: { date: "2025-05-07", time: "11:15", doctor: "Dr. Yosef", location: "Kfar Saba", occupied: false },
  appointment13: { date: "2025-05-08", time: "16:30", doctor: "Dr. Noy", location: "Petah Tikva", occupied: true },
  appointment14: { date: "2025-05-09", time: "17:00", doctor: "Dr. Golan", location: "Rishon", occupied: false },
  appointment15: { date: "2025-05-10", time: "18:00", doctor: "Dr. Bar", location: "Herzliya", occupied: true },
  appointment16: { date: "2025-05-11", time: "19:00", doctor: "Dr. Cohen", location: "Tel Aviv", occupied: false },
  appointment17: { date: "2025-05-12", time: "20:00", doctor: "Dr. Green", location: "Haifa", occupied: true },
  appointment18: { date: "2025-05-13", time: "21:00", doctor: "Dr. Lee", location: "Hod Hasharon", occupied: false },
  appointment19: { date: "2025-05-14", time: "22:00", doctor: "Dr. Kim", location: "Jerusalem", occupied: false },
  appointment20: { date: "2025-05-15", time: "23:00", doctor: "Dr. Amir", location: "Netanya", occupied: true },
  // Add more appointments as needed
  appointment21: { date: "2025-05-16", time: "08:00", doctor: "Dr. Levi", location: "Ashdod", occupied: false },
  appointment22: { date: "2025-05-17", time: "09:00", doctor: "Dr. Tal", location: "Eilat", occupied: true },
  appointment23: { date: "2025-05-18", time: "10:00", doctor: "Dr. Cohen", location: "Tiberias", occupied: false },
  appointment24: { date: "2025-06-19", time: "11:00", doctor: "Dr. Dana", location: "Nazareth", occupied: true },
  appointment25: { date: "2025-08-8", time: "12:00", doctor: "Dr. Peretz", location: "Ramat Gan", occupied: false },
  appointment26: { date: "2025-09-9", time: "12:00", doctor: "Dr. Peretz", location: "Ramat Gan", occupied: false },


};

// Upload appointments to Firebase
set(ref(db, "Appointments"), appointments)
  .then(() => {
    console.log("✅ Appointments uploaded successfully!");
  })
  .catch((error) => {
    console.error("❌ Failed to upload appointments:", error);
  });
