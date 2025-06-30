import subprocess
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from tkinter import filedialog


class NeuralNetworkConfigApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DNN2VHLS")
        self.root.geometry("900x700")
        
        # Dictionary to store accumulated layer configurations
        self.layer_configurations = {
            "Type": [],
            "Activation": [],
            "Neurons": [],
            "Padding": [],
            "KernelWidth": [],
            "KernelHeight": [],
            "VerticalStride": [],
            "HorizontalStride": []
        }
        
        # Dictionary to store model specifications
        self.model_specifications = {
            "DataWidth": 0.0,
            "IntegerBits": 0.0,
            "InputWidth": 0.0,
            "InputHeight": 0.0,
            "ModelName": ""
        }

        self.complete_configuration = {
            "LayerConfiguration": self.layer_configurations,
            "ModelSpecification": self.model_specifications,
            "LearnableParameters": ""
        }
        
        # Counter for unique layer IDs
        self.layer_counter = 0
        
        # Dictionary to store all layer frames and their data
        self.layer_frames = {}
        
        # Initialize the interface
        self.setup_interface()
    
    def setup_interface(self):
        """Initialize the complete interface"""
        # Create main notebook
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=10)
        
        # Create all tabs
        self.create_layer_config_tab()
        self.create_specifications_tab()
    
    def create_layer_config_tab(self):
        """Create the layer configuration tab"""
        # Create main frame for this tab
        layer_frame = ttk.Frame(self.notebook)
        self.notebook.add(layer_frame, text="Layer Configuration")
        
        # Title
        title_label = tk.Label(layer_frame, text="Layer Configuration Settings", 
                              font=("Arial", 14, "bold"), bg="#f8f9fa", pady=10)
        title_label.pack(fill="x")
        
        # Create scrollable area for layers
        self.setup_scrollable_area(layer_frame)
        
        # Create control buttons
        self.create_layer_control_buttons(layer_frame)
        
        # Create first layer
        self.add_new_layer()
    
    def setup_scrollable_area(self, parent):
        """Setup the scrollable area for layer configurations"""
        # Create frame to hold canvas and scrollbar
        canvas_container = tk.Frame(parent)
        canvas_container.pack(fill="both", expand=True, padx=10, pady=5)
        
        # Create canvas and scrollbar
        self.canvas = tk.Canvas(canvas_container, bg="#f8f9fa")
        self.scrollbar = ttk.Scrollbar(canvas_container, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = tk.Frame(self.canvas, bg="#f8f9fa")
        
        # Configure scrolling
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )
        
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        # Pack canvas and scrollbar
        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")
        
    
    def on_type_change(self, layer_data):
        """Handle type selection change to show/hide fields"""
        self.update_field_visibility(layer_data)
    
    def update_field_visibility(self, layer_data):
        """Update field visibility based on selected type"""
        selected_type = layer_data["vars"]["type"].get()
        
        # Fields to hide for FC type
        fc_hidden_fields = ["padding", "kernel_width", "kernel_height", "vertical_stride", "horizontal_stride"]
        
        if selected_type == "FC":
            # Hide FC-specific fields and set default values
            for field in fc_hidden_fields:
                row_key = f"{field}_row"
                if row_key in layer_data:
                    layer_data[row_key].pack_forget()  # Hide the row
                # Set default value of 1
                layer_data["vars"][field].set("1")
        else:
            # Show all fields for other types
            for field in fc_hidden_fields:
                row_key = f"{field}_row"
                if row_key in layer_data:
                    # Show the row by packing it back
                    layer_data[row_key].pack(fill="x", padx=10, pady=3)
                    # Move it to the correct position
                    self.reorder_fields(layer_data)
                # Clear the default value so user can enter their own
                if layer_data["vars"][field].get() == "1":
                    layer_data["vars"][field].set("")
    
    def reorder_fields(self, layer_data):
        """Reorder fields to maintain correct display order"""
        # Define the correct order
        field_order = ["type_row", "activation_row", "neurons_row", 
                       "padding_row", "kernel_width_row", "kernel_height_row", 
                       "vertical_stride_row", "horizontal_stride_row"]
        
        # Repack fields in correct order
        for field_key in field_order:
            if field_key in layer_data and layer_data[field_key].winfo_ismapped():
                layer_data[field_key].pack_forget()
                layer_data[field_key].pack(fill="x", padx=10, pady=3)
    
    
    def create_layer_control_buttons(self, parent):
        """Create control buttons for layer management"""
        button_frame = tk.Frame(parent, bg="#e9ecef", pady=10)
        button_frame.pack(fill="x", padx=10, pady=(5, 10))
        
        # Buttons
        buttons = [
            ("New Layer", self.add_new_layer, "#17a2b8"),
            ("Apply All Layers", self.apply_all_layers, "#28a745"),
            ("Show All Configs", self.show_all_configs, "#6c757d"),
            ("Clear All Configs", self.clear_all_configs, "#dc3545")
        ]
        
        for text, command, color in buttons:
            btn = tk.Button(button_frame, text=text, command=command,
                           bg=color, fg="white", font=("Arial", 11), padx=15)
            btn.pack(side="left", padx=5)
    
    def add_new_layer(self):
        """Add a new layer configuration"""
        self.layer_counter += 1
        layer_id = f"layer_{self.layer_counter}"
        
        # Create layer frame
        layer_data = self.create_layer_frame(layer_id, f"Layer {self.layer_counter}")
        
        # Store layer data
        self.layer_frames[layer_id] = layer_data
        
        # Update scroll region
        self.root.update_idletasks()
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))
    
    def create_layer_frame(self, layer_id, title):
        """Create a single layer configuration frame"""
        # Main container
        main_frame = tk.Frame(self.scrollable_frame, relief="raised", bd=2, bg="#ffffff")
        main_frame.pack(fill="x", padx=5, pady=5)
        
        # Header with title and delete button
        header_frame = tk.Frame(main_frame, bg="#e9ecef")
        header_frame.pack(fill="x")
        
        # Collapsible state
        expanded = tk.BooleanVar(value=True)
        
        # Left side - arrow and title
        left_header = tk.Frame(header_frame, bg="#e9ecef")
        left_header.pack(side="left", fill="x", expand=True)
        
        arrow_label = tk.Label(left_header, text="▼", bg="#e9ecef", font=("Arial", 10))
        arrow_label.pack(side="left", padx=5)
        
        title_label = tk.Label(left_header, text=title, bg="#e9ecef", 
                              font=("Arial", 11, "bold"))
        title_label.pack(side="left", padx=5)
        
        # Right side - delete button (if not first layer)
        if self.layer_counter > 1:
            delete_btn = tk.Button(header_frame, text="Delete", 
                                  command=lambda: self.delete_layer(layer_id),
                                  bg="#dc3545", fg="white", font=("Arial", 9))
            delete_btn.pack(side="right", padx=5, pady=2)
        
        # Content frame
        content_frame = tk.Frame(main_frame, bg="white")
        content_frame.pack(fill="x", pady=5)
        
        # Store references
        layer_data = {
            "main_frame": main_frame,
            "content_frame": content_frame,
            "vars": {},
            "expanded": expanded
        }
        
        # Create input fields
        self.create_layer_inputs(content_frame, layer_data)
        
        # Toggle functionality
        def toggle_layer():
            if expanded.get():
                content_frame.pack_forget()
                arrow_label.config(text="▶")
                expanded.set(False)
            else:
                content_frame.pack(fill="x", pady=5)
                arrow_label.config(text="▼")
                expanded.set(True)
        
        # Make header clickable
        for widget in [left_header, arrow_label, title_label]:
            widget.bind("<Button-1>", lambda e: toggle_layer())
            widget.config(cursor="hand2")
        
        return layer_data
    
    def create_layer_inputs(self, parent, layer_data):
        """Create input fields for a layer"""
        # Configuration for all fields
        fields = [
            ("Type", ["FC", "CONV", "MAXP", "MINP", "AVGP"], "combobox"),
            ("Activation", ["RELU", "ELU", "LEAKY", "CLIPED", "SIGMOID", "SOFTPLUS", "SWISH", "TANH", "NONE"], "combobox"),
            ("Neurons", None, "entry"),
            ("Padding", None, "entry"),
            ("Kernel Width", None, "entry"),
            ("Kernel Height", None, "entry"),
            ("Vertical Stride", None, "entry"),
            ("Horizontal Stride", None, "entry")
        ]
        
        # Create input fields
        for i, (field_name, values, field_type) in enumerate(fields):
            row_frame = tk.Frame(parent, bg="white")
            row_frame.pack(fill="x", padx=10, pady=3)
            
            # Label
            label = tk.Label(row_frame, text=f"{field_name}:", bg="white", 
                           width=18, anchor="w", font=("Arial", 10))
            label.pack(side="left")
            
            # Input widget
            if field_type == "combobox":
                var = tk.StringVar(value=values[0] if values else "")
                widget = ttk.Combobox(row_frame, textvariable=var, values=values,
                                    width=20, state="readonly")
                # Bind the Types combobox to handle field visibility
                if field_name == "Type":
                    widget.bind('<<ComboboxSelected>>', lambda e, ld=layer_data: self.on_type_change(ld))
            else:  # entry
                var = tk.StringVar()
                widget = tk.Entry(row_frame, textvariable=var, width=23, font=("Arial", 10))
            
            widget.pack(side="left", padx=5)
            
            # Store variable reference
            var_name = field_name.lower().replace(" ", "_")
            layer_data["vars"][var_name] = var
            
            # Store row frame reference for show/hide functionality
            row_name = f"{var_name}_row"
            layer_data[row_name] = row_frame
        
        # Status label
        status_label = tk.Label(parent, text="Ready to configure", 
                               bg="white", fg="gray", font=("Arial", 9))
        status_label.pack(pady=5)
        layer_data["status_label"] = status_label
        
        # Set initial visibility based on default type (FC)
        self.update_field_visibility(layer_data)
    
    def delete_layer(self, layer_id):
        """Delete a layer"""
        if len(self.layer_frames) <= 1:
            messagebox.showwarning("Warning", "Cannot delete the last layer!")
            return
        
        if messagebox.askyesno("Confirm", "Delete this layer configuration?"):
            # Remove from GUI
            self.layer_frames[layer_id]["main_frame"].destroy()
            
            # Remove from tracking
            del self.layer_frames[layer_id]
            
            # Update scroll region
            self.root.update_idletasks()
            self.canvas.configure(scrollregion=self.canvas.bbox("all"))
    
    def apply_all_layers(self):
        """Apply all layer configurations"""
        # Reset configurations
        self.layer_configurations = {
            "Type": [],
            "Activation": [],
            "Neurons": [],
            "Padding": [],
            "KernelWidth": [],
            "KernelHeight": [],
            "VerticalStride": [],
            "HorizontalStride": []
        }
        
        applied_count = 0
        errors = []
        
        for layer_id, layer_data in self.layer_frames.items():
            if self.validate_and_apply_layer(layer_id, layer_data):
                applied_count += 1
            else:
                errors.append(layer_id)
        
        # Show results
        if applied_count > 0:
            msg = f"Successfully applied {applied_count} layer configurations!"
            if errors:
                msg += f"\n\nFailed to apply {len(errors)} layers due to validation errors."
            messagebox.showinfo("Applied", msg)
        else:
            messagebox.showwarning("Warning", "No valid layer configurations to apply!")
        
        # Reset status messages after 3 seconds
        self.root.after(3000, self.reset_status_messages)
    
    def validate_and_apply_layer(self, layer_id, layer_data):
        """Validate and apply a single layer configuration"""
        # Get all values
        values = {}
        for var_name, var in layer_data["vars"].items():
            values[var_name] = var.get().strip() if var.get() else ""
        
        selected_type = values.get("type", "")
        
        # Define required fields based on type
        if selected_type == "FC":
            # For FC layers, only check basic fields
            required_fields = ["type", "activation", "neurons"]
            # Ensure hidden fields have default values
            hidden_fields = ["padding", "kernel_width", "kernel_height", "vertical_stride", "horizontal_stride"]
            for field in hidden_fields:
                if not values.get(field):
                    values[field] = "1"  # Set default value
        else:
            # For other types, all fields are required
            required_fields = ["type", "activation", "neurons", "padding", 
                              "kernel_width", "kernel_height", "vertical_stride", "horizontal_stride"]
        
        # Check for required fields
        missing_fields = []
        for field in required_fields:
            if not values.get(field):
                missing_fields.append(field.replace("_", " ").title())
        
        if missing_fields:
            layer_data["status_label"].config(
                text=f"❌ Missing: {', '.join(missing_fields)}", 
                fg="red", font=("Arial", 9)
            )
            return False
        
        # Validate numeric fields
        numeric_fields = ["neurons", "padding", "kernel_width", "kernel_height", 
                         "vertical_stride", "horizontal_stride"]
        
        validated_values = {}
        for field in numeric_fields:
            try:
                value = int(values[field])
                if value < 0:
                    layer_data["status_label"].config(
                        text=f"❌ {field.replace('_', ' ').title()} cannot be negative", 
                        fg="red", font=("Arial", 9)
                    )
                    return False
                validated_values[field] = value
            except ValueError:
                layer_data["status_label"].config(
                    text=f"❌ {field.replace('_', ' ').title()} must be a number", 
                    fg="red", font=("Arial", 9)
                )
                return False
        
        # All validation passed - add to configuration
        self.layer_configurations["Type"].append(values["type"])
        self.layer_configurations["Activation"].append(values["activation"])
        self.layer_configurations["Neurons"].append(validated_values["neurons"])
        self.layer_configurations["Padding"].append(validated_values["padding"])
        self.layer_configurations["KernelWidth"].append(validated_values["kernel_width"])
        self.layer_configurations["KernelHeight"].append(validated_values["kernel_height"])
        self.layer_configurations["VerticalStride"].append(validated_values["vertical_stride"])
        self.layer_configurations["HorizontalStride"].append(validated_values["horizontal_stride"])
        
        # Update status
        layer_data["status_label"].config(text="✓ Applied", fg="green", font=("Arial", 9, "bold"))
        return True
    
    def reset_status_messages(self):
        """Reset all status messages"""
        for layer_data in self.layer_frames.values():
            layer_data["status_label"].config(text="Ready to configure", fg="gray", font=("Arial", 9))
    
    def show_all_configs(self):
        """Show all current configurations"""
        if not self.layer_configurations["Type"]:
            messagebox.showinfo("Info", "No layer configurations applied yet!")
            return
        
        message = "Applied Layer Configurations:\n" + "="*20 + "\n\n"
        
        for i in range(len(self.layer_configurations["Type"])):
            message += f"Layer {i+1}:\n"
            message += f"  Type: {self.layer_configurations['Type'][i]}\n"
            message += f"  Activation: {self.layer_configurations['Activation'][i]}\n"
            message += f"  Neurons: {self.layer_configurations['Neurons'][i]}\n"
            if self.layer_configurations['Type'][i] != "FC":
                message += f"  Padding: {self.layer_configurations['Padding'][i]}\n"
                message += f"  Kernel Width: {self.layer_configurations['KernelWidth'][i]}\n"
                message += f"  Kernel Height: {self.layer_configurations['KernelHeight'][i]}\n"
                message += f"  Vertical Stride: {self.layer_configurations['VerticalStride'][i]}\n"
                message += f"  Horizontal Stride: {self.layer_configurations['HorizontalStride'][i]}\n\n"
        
        message += f"Total Layers: {len(self.layer_configurations['Type'])}"
        messagebox.showinfo("All Configurations", message)
    
    def clear_all_configs(self):
        """Clear all configurations"""
        if not self.layer_configurations["Types"]:
            messagebox.showinfo("Info", "No configurations to clear!")
            return
        
        if messagebox.askyesno("Confirm", f"Clear all {len(self.layer_configurations['Type'])} configurations?"):
            # Reset configurations
            self.layer_configurations = {
                "Type": [],
                "Activation": [],
                "Neurons": [],
                "Padding": [],
                "KernelWidth": [],
                "KernelHeight": [],
                "VerticalStride": [],
                "HorizontalStride": []
            }
            
            # Reset all status labels
            self.reset_status_messages()
            messagebox.showinfo("Cleared", "All configurations have been cleared!")
    
    def create_specifications_tab(self):
        """Create model specifications tab"""
        training_frame = ttk.Frame(self.notebook)
        self.notebook.add(training_frame, text="Model Specifications")
        
        # Title
        title = tk.Label(training_frame, text="Model Specification Settings", 
                        font=("Arial", 16, "bold"), pady=20)
        title.pack()
        
        # Settings frame
        settings_frame = tk.Frame(training_frame)
        settings_frame.pack(pady=20)
        
        # Store specification variables
        self.spec_vars = {}
        
        # Specification settings (all should be doubles/floats)
        settings = [
            ("Data Width:", "data_width"),
            ("Integer Bits:", "integer_bits"),
            ("Input Width:", "input_width"),
            ("Input Height:", "input_height"),
            ("Initiation Interval:", "initiation_interval")
        ]
        
        for i, (label_text, var_name) in enumerate(settings):
            tk.Label(settings_frame, text=label_text, font=("Arial", 12)).grid(
                row=i, column=0, sticky="w", padx=10, pady=5)
            
            # Create StringVar for each field
            var = tk.StringVar()
            entry = tk.Entry(settings_frame, textvariable=var, width=20, font=("Arial", 10))
            entry.grid(row=i, column=1, padx=10, pady=5)
            
            # Store the variable reference
            self.spec_vars[var_name] = var
        
        
        # Print everything button
        print_button = tk.Button(settings_frame, text="Upload CSV File and complete the configuration", 
                                command=self.print_everything_to_console,
                                bg="#28a745", fg="white", font=("Arial", 12), pady=5)
        print_button.grid(row=len(settings)+1, column=0, columnspan=2, pady=20)
    
    def print_everything_to_console(self):
        # Validate specifications
        specifications = {}
        validation_errors = []

        # Check each specification field (must be valid doubles)
        spec_field_names = {
            "data_width": "Data Width",
            "integer_bits": "Integer Bits", 
            "input_width": "Input Width",
            "input_height": "Input Height",
            "initiation_interval": "Initiation Interval",
        }

        for var_name, display_name in spec_field_names.items():
            value_str = self.spec_vars[var_name].get().strip()
            
            if not value_str:
                validation_errors.append(f"{display_name} is required")
                continue
            
            try:
                value = float(value_str)
                specifications[var_name] = value
            except ValueError:
                validation_errors.append(f"{display_name} must be a valid number")

        # Check if there's at least one layer configuration
        if not self.layer_configurations.get("Type"):
            validation_errors.append("At least one layer configuration is required")

        # Show validation errors if any (do NOT access specifications until here)
        if validation_errors:
            error_message = "Please fix the following issues:\n\n" + "\n".join(f"• {error}" for error in validation_errors)
            messagebox.showerror("Validation Error", error_message)
            return

        # Additional cross-field validation (now safe since all required fields exist)
        if int(specifications["integer_bits"]) > int(specifications["data_width"]):
            messagebox.showerror(
                "Validation Error",
                f"Integer Bits ({int(specifications['integer_bits'])}) must be less than Data Width ({int(specifications['data_width'])})"
            )
            return

        if int(specifications["initiation_interval"]) <= 0:
            messagebox.showerror(
                "Validation Error",
                "Initiation Interval must be a positive integer"
            )
            return
        
        # All validation passed - update specifications dictionary
        self.model_specifications = {
            "DataWidth": specifications["data_width"],
            "IntegerBits": specifications["integer_bits"],
            "InputWidth": specifications["input_width"],
            "InputHeight": specifications["input_height"],
            "InitiationInverval": specifications["initiation_interval"],
        }

        filename = filedialog.askopenfilename()
        if filename:
            with open(filename, 'r', encoding='utf-8') as f:
                contents = f.read()
                self.complete_configuration["LearnableParameters"] = contents 

        self.complete_configuration["LayerConfiguration"] = self.layer_configurations
        self.complete_configuration["ModelSpecification"] = self.model_specifications

        # result = subprocess.run(['generator --params'], capture_output=True, text=True)
        # result = subprocess.run(['run the actual program --params^'], capture_output=True, text=True)        
    
        cfg = self.complete_configuration
        print(f"LayerTypes = {cfg['LayerConfiguration']['Type']}")
        print(f"ActivationTypes = {cfg['LayerConfiguration']['Activations']}")
        print(f"Neurons = {cfg['LayerConfiguration']['Neurons']}")
        print(f"Padding = {cfg['LayerConfiguration']['Padding']}")
        print(f"kWidths = {cfg['LayerConfiguration']['KernelWidth']}")
        print(f"kHeights = {cfg['LayerConfiguration']['KernelHeight']}")
        print(f"vStrides = {cfg['LayerConfiguration']['VerticalStride']}")
        print(f"hStrides = {cfg['LayerConfiguration']['HorizontalStride']}")
        print(f"DataWidth = {cfg['ModelSpecification']['DataWidth']}")
        print(f"IntegerBits = {cfg['ModelSpecification']['IntegerBits']}")
        print(f"InputWidth = {cfg['ModelSpecification']['InputWidth']}")
        print(f"InputHeight = {cfg['ModelSpecification']['InputHeight']}")
        print(f"IIValue = {cfg['ModelSpecification']['InitiationInverval']}")
        print(f"Parameters = {cfg['LearnableParameters']}")

# Create and run the application
if __name__ == "__main__":
    root = tk.Tk()
    app = NeuralNetworkConfigApp(root)
    root.mainloop()