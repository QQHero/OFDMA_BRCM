(window.webpackJsonp=window.webpackJsonp||[]).push([["advStaticRoute"],{e8e6:function(t,e,a){"use strict";a.r(e);var i=a("05af"),s=a("710d"),o={mixins:[a("f07d").a],props:{lanInfo:{type:Object,default:()=>({})}},data(){return{rules:{gateway:[Object(s.e)(i.b),this.checkGateway],mask:[Object(s.e)(i.e)],network:[Object(s.e)(i.b),this.checkSameIp]}}},computed:{showModaltitle(){return this.isAdd?_("Add Static Route"):_("Edit Static Route")}},methods:{checkSameIp(){if(this.$refs.mask.checkValid()){if(this.updateNetwork(),this.tableData.some(({id:t,network:e,mask:a})=>{if((this.isAdd||this.formData.id!==t)&&Object(s.b)(this.formData.network,this.formData.mask,e,a))return!0}))return _("The destination network already exists")}},checkGateway(){return Object(s.d)(this.formData.gateway,"255.255.255.0")},beforeSubmit:t=>(t.ifname="WAN1",t),updateNetwork(){let t=this.formData.network.split("."),e=this.formData.mask.split("."),a=[];e.forEach((e,i)=>{a[i]=e&t[i]}),this.formData.network=a.join(".")},changeNetwork(){this.$nextTick(()=>{this.formData.mask&&this.formData.network&&this.$refs.mask.checkValid()&&this.$refs.network.checkValid()&&this.updateNetwork()})}}},r=a("0b56"),l=Object(r.a)(o,(function render(){var t=this,e=t._self._c;return e("v-dialog",{attrs:{title:t.showModaltitle,width:628},on:{confirm:t.submit},model:{value:t.showModal,callback:function(e){t.showModal=e},expression:"showModal"}},[e("div",[e("v-form",{ref:"form",attrs:{rules:t.rules,dialogCenter:!0}},[e("v-form-item",{ref:"network",attrs:{label:t._("Destination Network"),prop:"network"}},[e("v-input",{attrs:{maxlength:15},on:{change:t.changeNetwork},model:{value:t.formData.network,callback:function(e){t.$set(t.formData,"network",e)},expression:"formData.network"}})],1),e("v-form-item",{ref:"mask",attrs:{label:t._("Subnet Mask"),prop:"mask"}},[e("v-input",{attrs:{maxlength:15},on:{change:t.changeNetwork},model:{value:t.formData.mask,callback:function(e){t.$set(t.formData,"mask",e)},expression:"formData.mask"}})],1),e("v-form-item",{attrs:{label:t._("Gateway"),prop:"gateway",required:!1}},[e("v-input",{attrs:{maxlength:15},model:{value:t.formData.gateway,callback:function(e){t.$set(t.formData,"gateway",e)},expression:"formData.gateway"}})],1),e("v-form-item",{attrs:{label:"WAN",prop:"ifname"}},[t._v(" WAN1 ")])],1)],1)])}),[],!1,null,null,null).exports,n=a("719d");const c={id:"",network:"",mask:"",gateway:"",ifname:""};var d={mixins:[n.a],components:{staticDialog:l},props:{tableData:{type:Array,default:()=>[]}},data:()=>({maxNum:10,formData:Object(s.e)(c),formDefault:Object(s.e)(c),tableItem:[{key:"network",label:_("Destination Network")},{key:"mask",label:_("Subnet Mask")},{key:"gateway",label:_("Gateway")},{key:"ifname",label:"WAN"}]}),methods:{checkNum(){let t=this.tableData.filter(t=>"system"!==t.operateType).length;return!(this.maxNum&&t>=this.maxNum)||(this.$message.error(_("A maximum of %s rules are allowed",[this.maxNum])),!1)}}},m={components:{staticRoute:Object(r.a)(d,(function render(){var t=this,e=t._self._c;return e("div",{staticClass:"v-page-table"},[e("div",{staticClass:"v-page-table__title"},[t._v(t._s(t._("Routing Table")))]),e("div",{staticClass:"v-page-table__add-icon icon-add2",on:{click:t.addListEvent}}),e("v-table",{ref:"table",attrs:{data:t.tableData,"row-key":"mac",stripe:""}},[t._l(t.tableItem,(function(t){return e("v-table-col",{key:t.key,attrs:{prop:t.key,label:t.label,align:"center"}})})),e("v-table-col",{attrs:{prop:"option",label:t._("Operation"),align:"center"},scopedSlots:t._u([{key:"default",fn:function(a){return["system"==a.operateType?e("div",[t._v(t._s(t._("System")))]):e("div",[e("div",{directives:[{name:"tooltip",rawName:"v-tooltip",value:t._("Edit"),expression:"_('Edit')"}],staticClass:"icon-edit2 v-page-table__icon",on:{click:function(e){return t.editListEvent(a)}}}),e("v-popconfirm",{ref:"popconfirm",attrs:{icon:"v-icon-remind-plane popconfirm-custom-icon",title:t._("Do you want to delete the rule?"),clickOutsideToHide:!0},on:{confirm:function(e){return t.submitDel(a)}}},[e("div",{directives:[{name:"tooltip",rawName:"v-tooltip",value:t._("Delete"),expression:"_('Delete')"}],staticClass:"icon-delete v-page-table__icon",attrs:{slot:"reference"},slot:"reference"})])],1)]}}])})],2),e("static-dialog",{ref:"dialog",attrs:{tableData:t.tableData,option:t.option,formData:t.formData},on:{submitAdd:t.submitAdd,submitEdit:t.submitEdit}})],1)}),[],!1,null,null,null).exports},data:()=>({pageTitle:_("Static Routing"),pageTips:_("After a static route is added, data whose destination address is the same as the destination network of the static route will be directly forwarded according to the specified path."),isloading:!0,staticRouteList:[]}),created(){this.updateData()},methods:{init(){this.updateData()},updateData(){this.$getData({modules:"staticRouteList"}).then(t=>{this.staticRouteList=Object(s.e)(t.staticRouteList),Object(s.a)(this.staticRouteList),this.isloading=!1})},submit(){this.staticRouteList.forEach(t=>{delete t.id}),this.$postModule({staticRouteList:this.staticRouteList},_("Saved successfully")).then(()=>{this.updateData()})}}},u=Object(r.a)(m,(function render(){var t=this._self._c;return t("div",{staticClass:"route-set"},[t("v-page-title",{attrs:{title:this.pageTitle,tips:this.pageTips}}),t("v-loading",{attrs:{visible:this.isloading}}),this.isloading?this._e():t("static-route",{attrs:{tableData:this.staticRouteList},on:{submit:this.submit}})],1)}),[],!1,null,null,null);e.default=u.exports}}]);