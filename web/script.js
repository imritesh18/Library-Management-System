const API_BASE = "http://localhost:8080";
let demoMode = false;

const demoBooks = [
  {id:1,title:"The Alchemist",author:"Paulo Coelho",category:"Fiction",issued:false},
  {id:2,title:"Clean Code",author:"Robert C. Martin",category:"Programming",issued:true},
  {id:3,title:"Atomic Habits",author:"James Clear",category:"Self Help",issued:false},
  {id:4,title:"Introduction to Algorithms",author:"Cormen",category:"Computer Science",issued:false}
];

const demoMembers = [
  {id:1,name:"Rohan Sharma",email:"rohan@example.com"},
  {id:2,name:"Aman Verma",email:"aman@example.com"},
  {id:3,name:"Priya Singh",email:"priya@example.com"}
];

async function api(url, options = {}) {
  if (demoMode) throw new Error("demo mode");
  const response = await fetch(API_BASE + url, options);
  if (!response.ok) throw new Error("C++ server returned " + response.status);
  return response.json();
}

function updateStats() {
  const books = window.allBooks || [];
  const issued = books.filter(b => b.issued).length;
  document.getElementById("total").textContent = books.length;
  document.getElementById("available").textContent = books.length - issued;
  document.getElementById("issued").textContent = issued;
  document.getElementById("members").textContent = (window.allMembers || []).length;
}

function useDemoMode() {
  demoMode = true;
  window.allBooks = demoBooks;
  window.allMembers = demoMembers;
  updateStats();
  render();
}

async function load() {
  try {
    const [stats, books, members] = await Promise.all([
      api("/api/stats"), api("/api/books"), api("/api/members")
    ]);
    document.getElementById("total").textContent = stats.totalBooks;
    document.getElementById("available").textContent = stats.available;
    document.getElementById("issued").textContent = stats.issued;
    document.getElementById("members").textContent = stats.members;
    window.allBooks = books;
    window.allMembers = members;
    render();
  } catch {
    useDemoMode();
  }
}

function render() {
  const q = document.getElementById("search").value.toLowerCase();
  const list = (window.allBooks || []).filter(
    b => b.title.toLowerCase().includes(q) || b.author.toLowerCase().includes(q)
  );

  document.getElementById("books").innerHTML = list.map(b => `
    <tr>
      <td>${b.id}</td><td><b>${escapeHtml(b.title)}</b></td>
      <td>${escapeHtml(b.author)}</td><td>${escapeHtml(b.category)}</td>
      <td><span class="badge ${b.issued ? "issued" : "available"}">${b.issued ? "Issued" : "Available"}</span></td>
      <td><button onclick="toggle(${b.id})">${b.issued ? "Return" : "Issue"}</button></td>
    </tr>`).join("");

  document.getElementById("membersList").innerHTML =
    (window.allMembers || []).map(m => `
      <div class="member"><b>${escapeHtml(m.name)}</b><span>${escapeHtml(m.email)}</span></div>
    `).join("");
}

async function toggle(id) {
  if (demoMode) {
    const book = demoBooks.find(b => b.id === id);
    if (book) book.issued = !book.issued;
    updateStats(); render(); return;
  }
  try {
    await api("/api/toggle", {
      method:"POST",
      headers:{"Content-Type":"application/x-www-form-urlencoded"},
      body:"id="+encodeURIComponent(id)
    });
    await load();
  } catch { alert("Please start the C++ backend first."); }
}

function openBook(){ document.getElementById("bookModal").classList.add("show"); }
function openMember(){ document.getElementById("memberModal").classList.add("show"); }
function closeModal(id){ document.getElementById(id).classList.remove("show"); }

async function addBook(e) {
  e.preventDefault();
  const title=document.getElementById("title").value.trim();
  const author=document.getElementById("author").value.trim();
  const category=document.getElementById("category").value.trim();

  if (demoMode) {
    const id=demoBooks.length ? Math.max(...demoBooks.map(b=>b.id))+1 : 1;
    demoBooks.push({id,title,author,category,issued:false});
    window.allBooks=demoBooks; e.target.reset(); closeModal("bookModal");
    updateStats(); render(); return;
  }

  const body=new URLSearchParams({title,author,category});
  try {
    await api("/api/books", {
      method:"POST",
      headers:{"Content-Type":"application/x-www-form-urlencoded"},
      body:body.toString()
    });
    e.target.reset(); closeModal("bookModal"); await load();
  } catch { alert("C++ backend is not running. Start library_server.exe first."); }
}

async function addMember(e) {
  e.preventDefault();
  const name=document.getElementById("name").value.trim();
  const email=document.getElementById("email").value.trim();

  if (demoMode) {
    const id=demoMembers.length ? Math.max(...demoMembers.map(m=>m.id))+1 : 1;
    demoMembers.push({id,name,email});
    window.allMembers=demoMembers; e.target.reset(); closeModal("memberModal");
    updateStats(); render(); return;
  }

  const body=new URLSearchParams({name,email});
  try {
    await api("/api/members", {
      method:"POST",
      headers:{"Content-Type":"application/x-www-form-urlencoded"},
      body:body.toString()
    });
    e.target.reset(); closeModal("memberModal"); await load();
  } catch { alert("C++ backend is not running. Start library_server.exe first."); }
}

function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, ch => ({
    "&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#039;"
  }[ch]));
}

load();