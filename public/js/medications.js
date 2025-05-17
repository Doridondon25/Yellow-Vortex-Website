import { ref, set, update, remove, get } from "https://www.gstatic.com/firebasejs/9.22.0/firebase-database.js";
const db = window.db;

// Add medication
async function addMedication(userId, medData) {
  const newMedRef = ref(db, `users/${userId}/meds/${Date.now()}`);
  await set(newMedRef, {
    name: medData.name,
    dosage: medData.dosage,
    frequency: medData.frequency,
    timeToTake: medData.timeToTake
  });
}

// Edit medication
async function editMedication(userId, medId, medData) {
  const medRef = ref(db, `users/${userId}/meds/${medId}`);
  await update(medRef, medData);
}

// Delete medication
async function deleteMedication(userId, medId) {
  const medRef = ref(db, `users/${userId}/meds/${medId}`);
  await remove(medRef);
}

// Get all medications
async function getMedications(userId) {
  const medsRef = ref(db, `users/${userId}/meds`);
  const snapshot = await get(medsRef);
  return snapshot.val();
}

// Export functions
export {
  addMedication,
  editMedication,
  deleteMedication,
  getMedications
};