// Configuração do Firebase
const firebaseConfig = {
  apiKey: "AIzaSyAiiEqdMOtivQJJXaaAIOGVjQBWl_QWLak",
  authDomain: "meio-ambiente-12345.firebaseapp.com",
  projectId: "meio-ambiente-12345",
  storageBucket: "meio-ambiente-12345.appspot.com",
  messagingSenderId: "1050596917325",
  appId: "1:1050596917325:web:a86702c27c8a9cd211dc04",
  measurementId: "G-06DR64SJWZ",
};

const app = initializeApp(firebaseConfig);
const db = firebase.firestore();

// Função para atualizar dados da situação atual
function updateData() {
  const tempRef = db.collection("main").doc("temperatura");
  const umidadeRef = db.collection("main").doc("umidade");
  const co2Ref = db.collection("main").doc("co2");

  tempRef.onSnapshot((doc) => {
    if (doc.exists) {
      document.getElementById("temperatura").innerText =
        doc.data().value + "°C";
    }
  });

  umidadeRef.onSnapshot((doc) => {
    if (doc.exists) {
      document.getElementById("umidade").innerText = doc.data().value + "%";
    }
  });

  co2Ref.onSnapshot((doc) => {
    if (doc.exists) {
      document.getElementById("co2").innerText = doc.data().value + "%";
    }
  });
}

// Inicializar atualização dos dados
updateData();
