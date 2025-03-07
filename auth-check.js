import { initializeApp } from "https://www.gstatic.com/firebasejs/9.0.0/firebase-app.js";
import { getAuth, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/9.0.0/firebase-auth.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.0.0/firebase-database.js";

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
const auth = getAuth();
const database = getDatabase();

export function checkIsConnect() {
    onAuthStateChanged(auth, (user) => {
        if (user) {
            const userRef = ref(database, `users/${user.uid}/isConnect`);

            // Listen for changes to the `isConnect` field
            onValue(userRef, (snapshot) => {
                const isConnect = snapshot.val();
                if (isConnect === "0") {
                    alert("You have been logged out.");
                    window.location.href = "index.html";
                }
            });
        } else {
            // Redirect to login if no user is logged in
            window.location.href = "index.html";
        }
    });
}
